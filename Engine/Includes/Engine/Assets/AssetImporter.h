#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

namespace Engine
{
	class AssetImporter
	{
	public:
		static SharedPtr<Asset> ImportAsset(Asset::Type assetType, const std::filesystem::path& path);
		static SharedPtr<Asset> LoadAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}