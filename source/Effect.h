#pragma once

namespace dae
{
	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~Effect();

		Effect(const Effect&)				= delete;
		Effect(Effect&&)					= delete;
		Effect& operator=(const Effect&)	= delete;
		Effect& operator=(Effect&&)			= delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;

		void SetWorldViewProjectionMatrix(const Matrix& matrix);

		virtual ID3D11InputLayout* LoadInputLayout(ID3D11Device* pDevice) = 0;
	protected:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};


		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile) const;
	};
}

