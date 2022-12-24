#pragma once

namespace dae
{
	class Texture;
	class Effect;

	/*struct Vertex final
	{
		Vector3 position;
		ColorRGB color;
	};*/
	struct Vertex final
	{
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector2 uv;
	};

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::string& resourcePath, Effect* pEffect, ID3D11SamplerState* pSampleState = nullptr);
		~Mesh();

		Mesh(const Mesh&)				= delete;
		Mesh(Mesh&&)					= delete;
		Mesh& operator=(const Mesh)		= delete;
		Mesh& operator=(Mesh&&)			= delete;

		void Render(ID3D11DeviceContext* pDeviceContext) const;
		bool IsVisible() const;

		void SetPosition(const Vector3& position);
		void RotateY(float rotation);
		void SetMatrices(const Matrix& viewProjectionMatrix, const Matrix& inverseViewMatrix);
		void SetSamplerState(ID3D11SamplerState* pSampleState);
		void SetVisibility(bool isVisible);
	private:
		bool m_IsVisible{ true };
		Matrix m_TranslationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_RotationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_ScaleMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };

		Texture* m_pDiffuseTexture{};
		Texture* m_pNormalTexture{};
		Texture* m_pSpecularTexture{};
		Texture* m_pGlossinessTexture{};

		Effect* m_pEffect{};

		ID3D11InputLayout* m_pInputLayout{};

		ID3D11Buffer* m_pVertexBuffer{};

		uint32_t m_NumIndices{};
		ID3D11Buffer* m_pIndexBuffer{};
	};
}
