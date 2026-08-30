#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "Engine/Core/Core.h"
#include "Engine/Rendering/Material.h"

namespace Engine
{
	class MaterialImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static Ref<Material> LoadMaterial(AssetHandle handle, const AssetMetadata& metadata);

		// Reads file directly from filesystem 
		// (i.e path has to be relative/absolute to working directory
		static Ref<Material> ImportMaterial(const std::filesystem::path& path);
	};
}