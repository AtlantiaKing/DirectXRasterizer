#include "pch.h"
#include "EffectPosTex.h"
#include "Texture.h"

namespace dae
{
	dae::EffectPosTex::EffectPosTex(ID3D11Device* pDevice, const std::wstring& assetFile)
		: Effect{ pDevice, assetFile }
	{
		// Save the diffuse texture variable of the effect as a member variable
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) std::wcout << L"m_pDiffuseMapVariable not valid\n";
	}

	void dae::EffectPosTex::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		// Set the shader resource view of the texture to the texture variable of the effect
		if (m_pDiffuseMapVariable)
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
	}

	ID3D11InputLayout* dae::EffectPosTex::LoadInputLayout(ID3D11Device* pDevice)
	{
		// Create vertex layout
		static constexpr uint32_t numElements{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		// Create input layout
		D3DX11_PASS_DESC passDesc{};
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		ID3D11InputLayout* pInputLayout;

		HRESULT result{ pDevice->CreateInputLayout
			(
				vertexDesc,
				numElements,
				passDesc.pIAInputSignature,
				passDesc.IAInputSignatureSize,
				&pInputLayout
			) };
		if (FAILED(result)) return nullptr;

		return pInputLayout;
	}
}