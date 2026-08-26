#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "StaticMeshAsset.h"
#include "Engine/Core/Core.h"

#include <filesystem>

namespace Engine
{
	class MeshImporter
	{
	public:
		static Ref<StaticMeshAsset> LoadStaticMesh(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<StaticMeshAsset> ImportStaticMesh(const std::filesystem::path& assetPath);
	};
}