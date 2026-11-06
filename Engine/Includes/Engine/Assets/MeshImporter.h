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
		static SharedPtr<StaticMeshAsset> LoadStaticMesh(AssetHandle handle, const AssetMetadata& metadata);
		static SharedPtr<StaticMeshAsset> ImportStaticMesh(const std::filesystem::path& assetPath);
	};
}