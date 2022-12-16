#include "pch.h"
#include "EffectPosCol.h"

namespace dae
{
	EffectPosCol::EffectPosCol(ID3D11Device* pDevice, const std::wstring& assetFile)
		: Effect{ pDevice, assetFile }
	{
	}

	ID3D11InputLayout* EffectPosCol::LoadInputLayout(ID3D11Device* pDevice)
	{
		static constexpr uint32_t numElements{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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