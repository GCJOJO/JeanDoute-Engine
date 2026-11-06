#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Assets/AssetMetadata.h"
#include "Engine/Rendering/Texture.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static SharedPtr<Texture2D> LoadTexture2D(AssetHandle handle, const AssetMetadata& metadata);

		// Reads file directly from filesystem 
		// (i.e path has to be relative/absolute to working directory
		static SharedPtr<Texture2D> ImportTexture2D(const std::filesystem::path& path);
	};
}