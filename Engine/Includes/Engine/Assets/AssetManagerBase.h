#pragma once
#include "Asset.h"
#include "Engine/Core/Core.h"

#include <map>

namespace Engine
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual Ref<Asset> GetAsset(const std::filesystem::path& filepath) = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual Asset::Type GetAssetType(AssetHandle handle) const = 0;
	};
}