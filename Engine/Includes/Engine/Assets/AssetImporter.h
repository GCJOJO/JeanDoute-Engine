#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

namespace Engine
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(Asset::Type assetType, const std::filesystem::path& path);
		static Ref<Asset> LoadAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}