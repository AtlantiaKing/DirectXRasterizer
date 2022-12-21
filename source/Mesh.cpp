#include "pch.h"
#include "Mesh.h"
#include "Texture.h"
#include "EffectPosTex.h"
#include "Utils.h"

namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, const std::string& resourcePath, ID3D11SamplerState* pSampleState)
		: m_pEffect{ new EffectPosTex{ pDevice, L"Resources/PosTex3D.fx" } }
	{
		// Load vertices and indices from a file
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		bool parseResult{ Utils::ParseOBJ(resourcePath, vertices, indices) };
		if (!parseResult)
		{
			std::cout << "Failed to load OBJ from " << resourcePath << "\n";
			return;
		}

		// Create diffuse texture
		m_pDiffuseTexture = Texture::LoadFromFile(pDevice, "Resources/vehicle_diffuse.png", Texture::TextureType::Diffuse);
		m_pEffect->SetTexture(m_pDiffuseTexture);

		// Create normal texture
		m_pNormalTexture = Texture::LoadFromFile(pDevice, "Resources/vehicle_normal.png", Texture::TextureType::Normal);
		m_pEffect->SetTexture(m_pNormalTexture);

		// Create specular texture
		m_pSpecularTexture = Texture::LoadFromFile(pDevice, "Resources/vehicle_specular.png", Texture::TextureType::Specular);
		m_pEffect->SetTexture(m_pSpecularTexture);

		// Create glossiness texture
		m_pGlossinessTexture = Texture::LoadFromFile(pDevice, "Resources/vehicle_gloss.png", Texture::TextureType::Glossiness);
		m_pEffect->SetTexture(m_pGlossinessTexture);

		// Create Input Layout
		m_pInputLayout = m_pEffect->LoadInputLayout(pDevice);

		// Create vertex buffer
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		HRESULT result{ pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer) };
		if (FAILED(result)) return;

		// Create index buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result)) return;

		if (pSampleState) SetSamplerState(pSampleState);
	}

	Mesh::~Mesh()
	{
		if(m_pIndexBuffer) m_pIndexBuffer->Release();
		if (m_pVertexBuffer) m_pVertexBuffer->Release();

		if (m_pInputLayout) m_pInputLayout->Release();

		delete m_pDiffuseTexture;
		delete m_pNormalTexture;
		delete m_pSpecularTexture;
		delete m_pGlossinessTexture;

		delete m_pEffect;
	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
	{
		// Set primitive topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set input layout
		pDeviceContext->IASetInputLayout(m_pInputLayout);

		// Set vertex buffer
		constexpr UINT stride{ sizeof(Vertex) };
		constexpr UINT offset{ 0 };
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		// Set index buffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Draw
		D3DX11_TECHNIQUE_DESC techniqueDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techniqueDesc);
		for (UINT p{}; p < techniqueDesc.Passes; ++p)
		{
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}

	void Mesh::SetPosition(const Vector3& position)
	{
		m_TranslationMatrix = Matrix::CreateTranslation(position);
	}

	void Mesh::RotateY(float rotation)
	{
		m_RotationMatrix = Matrix::CreateRotationY(rotation) * m_RotationMatrix;
	}

	void Mesh::SetMatrices(const Matrix& viewProjectionMatrix, const Matrix& inverseViewMatrix)
	{
		Matrix worldMatrix{ m_ScaleMatrix * m_RotationMatrix * m_TranslationMatrix };
		m_pEffect->SetWorldViewProjectionMatrix(worldMatrix * viewProjectionMatrix);
		m_pEffect->SetInverseViewMatrix(inverseViewMatrix);
		m_pEffect->SetWorldMatrix(worldMatrix);
	}

	void Mesh::SetSamplerState(ID3D11SamplerState* pSampleState)
	{
		ID3DX11EffectSamplerVariable* pSamplerStateVariable{ m_pEffect->GetEffect()->GetVariableByName("gSamState")->AsSampler() };

		HRESULT hr{ pSamplerStateVariable->SetSampler(0, pSampleState) };
		if (FAILED(hr)) std::wcout << L"Failed to change sample state";
	}
}