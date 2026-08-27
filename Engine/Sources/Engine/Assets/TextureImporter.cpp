#include "enginepch.h"
#include "Engine/Core/Profiler.h"
#include "Engine/Assets/TextureImporter.h"
#include "Engine/Project/Project.h"

#include <stb_image.h>

namespace Engine
{
	Ref<Texture2D> TextureImporter::LoadTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		ENGINE_PROFILE_FUNCTION();

		return ImportTexture2D(Project::GetActiveAssetDirectory() / metadata.AssetPath);
	}

	Ref<Texture2D> TextureImporter::ImportTexture2D(const std::filesystem::path& path)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		Buffer data;
		{
			ENGINE_PROFILE_SCOPE("stbi_load - TextureImporter::LoadTexture2D");
			std::string pathStr = path.string();
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
			channels = 4;
		}

		if (data.Data == nullptr)
		{
			ENGINE_CORE_ERROR("TextureImporter::LoadTexture2D - Couldn't load texture from filepath : {}", path.string());
			return nullptr;
		}

		// TODO: Think about this
		data.Size = width * height * channels;

		TextureSpecifications spec;
		spec.Width = width;
		spec.Height = height;
		switch (channels)
		{
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
		}

		Ref<Texture2D> texture = Texture2D::Create(spec, data);
		data.Release();
		return texture;
	}

}