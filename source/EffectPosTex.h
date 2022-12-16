#pragma once
#include "Effect.h"

namespace dae
{
	class Texture;

	class EffectPosTex final : public Effect
	{
	public:
		EffectPosTex(ID3D11Device* pDevice, const std::wstring& assetFile);

		void SetDiffuseMap(Texture* pDiffuseTexture);

		virtual ID3D11InputLayout* LoadInputLayout(ID3D11Device* pDevice) override;
	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	};
}

