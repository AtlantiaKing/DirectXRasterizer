#pragma once

namespace dae
{
	class Texture;

	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~Effect();

		Effect(const Effect&)				= delete;
		Effect(Effect&&)					= delete;
		Effect& operator=(const Effect&)	= delete;
		Effect& operator=(Effect&&)			= delete;

		virtual void SetMatrix(MatrixType type, const Matrix& matrix);
		virtual void SetTexture(Texture* pTexture) = 0;
		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;

		ID3D11InputLayout* LoadInputLayout(ID3D11Device* pDevice);
	protected:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};


		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile) const;
	};
}

