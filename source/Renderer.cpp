#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		LoadSampleState(D3D11_FILTER_MIN_MAG_MIP_POINT);

		m_pMesh = new Mesh{ m_pDevice, "Resources/vehicle.obj", m_pSampleState};

		m_pCamera = new Camera{};
		m_pCamera->Initialize(45.0f, { 0.0f, 0.0f, -50.0f }, static_cast<float>(m_Width) / m_Height);
	}

	Renderer::~Renderer()
	{
		delete m_pCamera;
		delete m_pMesh;

		if (m_pSampleState) m_pSampleState->Release();

		if (m_pRenderTargetView) m_pRenderTargetView->Release();
		if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();

		if (m_pDepthStencilView) m_pDepthStencilView->Release();
		if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();

		if (m_pSwapChain) m_pSwapChain->Release();

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice) m_pDevice->Release();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);

		const float rotateSpeed{ 45.0f };
		m_pMesh->RotateY(rotateSpeed * TO_RADIANS * pTimer->GetElapsed());

		UpdateWorldViewProjection();
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		// 1. Clear RTV and DSV
		ColorRGB clearColor{ 0.0f, 0.0f, 0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// 2. Set pipeline + Invoke drawcalls (= render)
		m_pMesh->Render(m_pDeviceContext);

		// 3. Present backbuffer (swap)
		m_pSwapChain->Present(0, 0);
	}

	void Renderer::ToggleRenderSampleState()
	{
		m_SampleState = static_cast<SampleState>((static_cast<int>(m_SampleState) + 1) % (static_cast<int>(SampleState::Anisotropic) + 1));

		D3D11_FILTER newFilter{};
		std::cout << "Changed sample state to: ";
		switch (m_SampleState)
		{
		case dae::Renderer::SampleState::Point:
			std::cout << "Point\n";
			newFilter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case dae::Renderer::SampleState::Linear:
			std::cout << "Linear\n";
			newFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case dae::Renderer::SampleState::Anisotropic:
			std::cout << "Anisotropic\n";
			newFilter = D3D11_FILTER_ANISOTROPIC;
			break;
		}

		LoadSampleState(newFilter);

		m_pMesh->SetSamplerState(m_pSampleState);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		// 1. Create Device and DeviceContext
		D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_1 };
		uint32_t createDeviceFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result{ D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext) };
		if (FAILED(result)) return result;

		// 2. Create Swapchain
		// 
		// Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result)) return result;

		// Create the swapchain description
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get the handle (HWND) from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		// Create swapchain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		pDxgiFactory->Release();

		if (FAILED(result)) return result;

		// 3. Create DepthStencil (DS) and DepthStencilView (DSV)
		// Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		// View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the stencil buffer
		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result)) return result;

		// Create the stencil view
		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result)) return result;

		// 4. Create RenderTarget (RT) and RenderTargetView (RTV)
		// Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result)) return result;

		// View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result)) return result;

		// 5. Bind RTV and DSV to Output Merger Stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// 6. Set viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return S_OK;
	}

	void Renderer::LoadSampleState(D3D11_FILTER filter)
	{
		D3D11_SAMPLER_DESC sampleDesc{};
		sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampleDesc.MipLODBias = 0;
		sampleDesc.MinLOD = 0;
		sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
		sampleDesc.MaxAnisotropy = 16;
		sampleDesc.Filter = filter;

		if (m_pSampleState) m_pSampleState->Release();

		HRESULT hr{ m_pDevice->CreateSamplerState(&sampleDesc, &m_pSampleState) };
		if (FAILED(hr)) std::wcout << L"m_pSampleState failed to load\n";
	}

	void Renderer::UpdateWorldViewProjection()
	{
		const Matrix ViewProjMatrix{ m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix() };

		m_pMesh->SetMatrices(ViewProjMatrix, m_pCamera->GetInverseViewMatrix());
	}
}
