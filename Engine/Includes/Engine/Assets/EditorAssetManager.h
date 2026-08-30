#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"

namespace Engine
{
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual Ref<Asset> GetAsset(const std::filesystem::path& filepath) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		bool IsAssetLoaded(const std::filesystem::path& filepath) const;

		virtual Asset::Type GetAssetType(AssetHandle handle) const override;

		Ref<Asset> ImportAsset(const std::filesystem::path& filepath);

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();


	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

		// TODO: memory only assets
	};
}