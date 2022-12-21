#pragma once
#include "Effect.h"

namespace dae
{
	class Texture;

	class EffectPosTex final : public Effect
	{
	public:
		EffectPosTex(ID3D11Device* pDevice, const std::wstring& assetFile);

		void SetTexture(Texture* pTexture);
		void SetWorldMatrix(const Matrix& worldMatrix);
		void SetInverseViewMatrix(const Matrix& viewMatrix);

		virtual ID3D11InputLayout* LoadInputLayout(ID3D11Device* pDevice) override;
	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};

		ID3DX11EffectMatrixVariable* m_pMatWorldVariable{};
		ID3DX11EffectMatrixVariable* m_pMatInverseViewVariable{};
	};
}

