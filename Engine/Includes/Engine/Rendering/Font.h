#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Rendering/Texture.h"

#include <filesystem>
#include <memory>

namespace Engine
{
	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();

		const MSDFData* GetMSDFData() const { return m_Data; }
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }
		const std::string& GetFontName() const { return m_FontName; }

		static Ref<Font> GetDefault();

	private:
		std::string m_FontName;
		MSDFData* m_Data = nullptr;
		Ref<Texture2D> m_AtlasTexture;
	};
}