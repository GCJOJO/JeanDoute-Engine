#pragma once
#include "Engine/Core/Buffer.h"
#include "Engine/Assets/Asset.h"

#include "bgfx/bgfx.h"
#include <cstdint>

namespace Engine
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecifications
	{
		uint32_t Width = 1;
		uint64_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	static bgfx::TextureFormat::Enum ToBgfxTextureFormat(ImageFormat format);

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureSpecifications& GetSpecifications() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual bgfx::TextureHandle GetTextureHandle() const = 0;

		virtual void Bind(bgfx::UniformHandle textureUniform) = 0;
		virtual void SetData(Buffer data) = 0;
		virtual bool IsLoaded() const = 0;
		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecifications& specifications, Buffer data = Buffer());
		~Texture2D();

		virtual Asset::Type GetType() const { return Asset::Type::Texture2D; }

		const TextureSpecifications& GetSpecifications() const override { return m_Specifications; };
		uint32_t GetWidth() const override { return m_Specifications.Width; };
		uint32_t GetHeight() const override { return m_Specifications.Height; };
		bgfx::TextureHandle GetTextureHandle() const override { return m_TextureHandle; };

		void Bind(bgfx::UniformHandle textureUniform) override;
		void SetData(Buffer data) override;
		bool IsLoaded() const override;
		bool operator ==(const Texture& other) const override { return GetTextureHandle().idx == other.GetTextureHandle().idx; }

	private:
		TextureSpecifications m_Specifications;
		bgfx::TextureHandle m_TextureHandle;
	};
}