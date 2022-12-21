#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
	struct Vector2;

	class Texture final
	{
	public:
		enum class TextureType
		{
			Diffuse,
			Normal,
			Specular,
			Glossiness
		};

		~Texture();

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path, TextureType type);

		ID3D11Texture2D* GetResource() const;
		ID3D11ShaderResourceView* GetSRV() const;
		TextureType GetType() const;
	private:
		Texture(ID3D11Device* pDevice, SDL_Surface* pSurface, TextureType type);

		TextureType m_Type{};
		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pSRV{};
	};
}