#pragma once
#include "Effect.h"

namespace dae
{
	class Texture;

	class EffectPosCol final : public Effect
	{
	public:
		EffectPosCol(ID3D11Device* pDevice, const std::wstring& assetFile);

		virtual void SetTexture(Texture* pTexture) override {};
	};
}