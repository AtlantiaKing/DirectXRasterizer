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

		// Save the normal texture variable of the effect as a member variable
		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid()) std::wcout << L"m_pNormalMapVariable not valid\n";

		// Save the specular texture variable of the effect as a member variable
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid()) std::wcout << L"m_pSpecularMapVariable not valid\n";

		// Save the glossiness texture variable of the effect as a member variable
		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid()) std::wcout << L"m_pGlossinessMapVariable not valid\n";

		// Save the worldmatrix variable of the effect as a member variable
		m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
		if (!m_pMatWorldVariable->IsValid()) std::wcout << L"m_pMatWorldVariable not valid\n";

		// Save the inverseviewmatrix variable of the effect as a member variable
		m_pMatInverseViewVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
		if (!m_pMatInverseViewVariable->IsValid()) std::wcout << L"m_pMatInverseViewVariable not valid\n";
	}

	void EffectPosTex::SetTexture(Texture* pTexture)
	{
		ID3DX11EffectShaderResourceVariable* pCurMapVariable{};

		switch (pTexture->GetType())
		{
		case dae::Texture::TextureType::Diffuse:
			pCurMapVariable = m_pDiffuseMapVariable;
			break;
		case dae::Texture::TextureType::Normal:
			pCurMapVariable = m_pNormalMapVariable;
			break;
		case dae::Texture::TextureType::Specular:
			pCurMapVariable = m_pSpecularMapVariable;
			break;
		case dae::Texture::TextureType::Glossiness:
			pCurMapVariable = m_pGlossinessMapVariable;
			break;
		}

		// Set the shader resource view of the texture to the texture variable of the effect
		if (pCurMapVariable)
			pCurMapVariable->SetResource(pTexture->GetSRV());
	}

	void EffectPosTex::SetWorldMatrix(const Matrix& worldMatrix)
	{
		m_pMatWorldVariable->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
	}

	void EffectPosTex::SetInverseViewMatrix(const Matrix& viewMatrix)
	{
		m_pMatInverseViewVariable->SetMatrix(reinterpret_cast<const float*>(&viewMatrix));
	}

	ID3D11InputLayout* dae::EffectPosTex::LoadInputLayout(ID3D11Device* pDevice)
	{
		// Create vertex layout
		static constexpr uint32_t numElements{ 4 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "NORMAL";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 12;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TANGENT";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 24;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TEXCOORD";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[3].AlignedByteOffset = 36;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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