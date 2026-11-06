#pragma once

#include "AssetManagerBase.h"

#include "Engine/Project/Project.h"

namespace Engine
{
	// Static API Class
	class AssetManager
	{
	public:
		template<typename T>
		static SharedPtr<T> GetAsset(AssetHandle handle)
		{
			// Get out project asset manager
			SharedPtr<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static bool IsAssetHandleValid(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetHandleValid(handle);
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetLoaded(handle);
		}

		static Asset::Type GetAssetType(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->GetAssetType(handle);
		}
	};
}