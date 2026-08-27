#include "enginepch.h"
#include "Engine/Rendering/Texture.h"

namespace Engine
{
	bgfx::TextureFormat::Enum ToBgfxTextureFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::R8:
			return bgfx::TextureFormat::R8;
		case ImageFormat::RGB8:
			return bgfx::TextureFormat::RGB8;
		case ImageFormat::RGBA8:
			return bgfx::TextureFormat::RGBA8;
		case ImageFormat::RGBA32F:
			return bgfx::TextureFormat::RGBA32F;
		}

		return bgfx::TextureFormat::Unknown;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecifications& specifications, Buffer data)
	{
		Ref<Texture2D> newTexture = CreateRef<Texture2D>();
		newTexture->m_Specifications = specifications;
		
		newTexture->m_TextureHandle = bgfx::createTexture2D(specifications.Width, specifications.Height, 
			false,
			1, ToBgfxTextureFormat(specifications.Format), 
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

		newTexture->SetData(data);
		return newTexture;
	}

	Texture2D::~Texture2D()
	{
		/*if (bgfx::isValid(m_TextureHandle))
			bgfx::destroy(m_TextureHandle);*/
	}

	void Texture2D::Bind(bgfx::UniformHandle textureUniform)
	{
		bgfx::setTexture(0, textureUniform, m_TextureHandle);
	}

	void Texture2D::SetData(Buffer data)
	{
		bgfx::updateTexture2D(m_TextureHandle, 0, 0, 0, 0, m_Specifications.Width, m_Specifications.Height,
			bgfx::copy(data.Data, data.Size));
	}

	bool Texture2D::IsLoaded() const
	{
		return bgfx::isValid(m_TextureHandle);
	}

}