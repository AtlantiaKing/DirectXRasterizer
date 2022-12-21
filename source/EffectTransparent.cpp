#include "pch.h"
#include "EffectTransparent.h"
#include "Texture.h"

namespace dae
{
	dae::EffectTransparent::EffectTransparent(ID3D11Device* pDevice, const std::wstring& assetFile)
		: Effect{ pDevice, assetFile }
	{
		// Save the diffuse texture variable of the effect as a member variable
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) std::wcout << L"m_pDiffuseMapVariable not valid\n";
	}

	void EffectTransparent::SetTexture(Texture* pTexture)
	{
		m_pDiffuseMapVariable->SetResource(pTexture->GetSRV());
	}
}
