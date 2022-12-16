#pragma once
#include "Effect.h"

namespace dae
{
	class EffectPosCol final : public Effect
	{
	public:
		EffectPosCol(ID3D11Device* pDevice, const std::wstring& assetFile);

		virtual ID3D11InputLayout* LoadInputLayout(ID3D11Device* pDevice) override;
	};
}