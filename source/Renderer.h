#pragma once

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Mesh;
	class Camera;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void ToggleRenderSampleState();
		void SetVisibility(int index, bool isVisible);
		void ToggleVisibilty(int index);
		void ToggleMeshRotation();

	private:
		enum class SampleState
		{
			Point,
			Linear,
			Anisotropic
		};

		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		Camera* m_pCamera{};
		std::vector<Mesh*> m_pMeshes{};
		SampleState m_SampleState{ SampleState::Point };
		bool m_IsRotatingMesh{ true };

		//DIRECTX
		ID3D11SamplerState* m_pSampleState{};
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		HRESULT InitializeDirectX();
		void LoadMeshes();
		void LoadSampleState(D3D11_FILTER filter);
		void UpdateWorldViewProjection();
	};
}
