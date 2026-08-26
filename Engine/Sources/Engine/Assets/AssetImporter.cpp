#include "enginepch.h"
#include "Engine/Assets/AssetImporter.h"
#include "Engine/Assets/MeshImporter.h"

namespace Engine
{
	using AssetImportFunction = std::function<Ref<Asset>(const std::filesystem::path&)>;
	static std::map<Asset::Type, AssetImportFunction> s_AssetImportingFunctions =
	{
		{ Asset::Type::StaticMesh, MeshImporter::ImportStaticMesh }
	};

	Ref<Asset> AssetImporter::ImportAsset(Asset::Type assetType, const std::filesystem::path& path)
	{
		if (s_AssetImportingFunctions.find(assetType) == s_AssetImportingFunctions.end())
		{
			ENGINE_CORE_ERROR("No importer available for Asset Type : {}", (uint16_t)assetType);
			return nullptr;
		}

		return s_AssetImportingFunctions.at(assetType)(path);
	}

	using AssetLoadFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<Asset::Type, AssetLoadFunction> s_AssetLoadingFunctions =
	{
		//{ Asset::Type::Scene, SceneImporter::ImportScene },
		//{ Asset::Type::Texture2D, TextureImporter::ImportTexture2D },
		{ Asset::Type::StaticMesh, MeshImporter::LoadStaticMesh }
	};

	Ref<Asset> AssetImporter::LoadAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetLoadingFunctions.find(metadata.Type) == s_AssetLoadingFunctions.end())
		{
			ENGINE_CORE_ERROR("No importer available for Asset Type : {}", (uint16_t)metadata.Type);
			return nullptr;
		}

		return s_AssetLoadingFunctions.at(metadata.Type)(handle, metadata);
	}

}