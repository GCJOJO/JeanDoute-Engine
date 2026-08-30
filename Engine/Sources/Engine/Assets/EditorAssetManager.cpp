#include "enginepch.h"
#include "Engine/Assets/EditorAssetManager.h"
#include "Engine/Assets/AssetImporter.h"
#include "Engine/Project/Project.h"

#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Engine
{

	static std::map<std::filesystem::path, Asset::Type> s_AssetExtensionMap =
	{
		{ ".scene", Asset::Type::Scene },
		{ ".obj", Asset::Type::StaticMesh },
		{ ".fbx", Asset::Type::StaticMesh },
		{ ".glb", Asset::Type::StaticMesh },
		{ ".gltf", Asset::Type::StaticMesh },

		{ ".png", Asset::Type::Texture2D },
		{ ".jpg", Asset::Type::Texture2D },
		{ ".jpeg", Asset::Type::Texture2D },
	};

	static Asset::Type GetTypeFromFileExtension(const std::filesystem::path& extension)
	{
		auto it = s_AssetExtensionMap.find(extension);
		if (it == s_AssetExtensionMap.end())
		{
			ENGINE_CORE_ERROR("Couldn't not find AssetType for extension {}", extension.string());
			return Asset::Type::None;
		}

		return it->second;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		// 1. Check is handle is valid
		// 2. Check is asset needs load (and if so, load)
		// 3. Return asset

		if (!IsAssetHandleValid(handle))
			return nullptr;

		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
			asset = m_LoadedAssets.at(handle);
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::LoadAsset(handle, metadata);
			if (asset)
				m_LoadedAssets[handle] = asset;
			else
				// Import Failed
				ENGINE_CORE_ERROR("EditorAssetManager::GetAsset - Asset Import Failed !");
		}
		return asset;
	}

	Ref<Asset> EditorAssetManager::GetAsset(const std::filesystem::path& filepath)
	{
		if (IsAssetLoaded(filepath))
		{
			for (const auto& asset : m_AssetRegistry)
			{
				if (asset.second.AssetPath == filepath)
					return GetAsset(asset.first);
			}
		}

		return ImportAsset(filepath);
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	bool EditorAssetManager::IsAssetLoaded(const std::filesystem::path& filepath) const
	{
		for (const auto& asset : m_AssetRegistry)
		{
			if (asset.second.AssetPath == filepath)
				return true;
		}

		return false;
	}

	Asset::Type EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return Asset::Type::None;
		return m_AssetRegistry.at(handle).Type;
	}

	Ref<Asset> EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		if (IsAssetLoaded(filepath))
			return nullptr;

		AssetHandle handle; // Generate new handle
		AssetMetadata metadata;
		metadata.AssetPath = filepath;
		try {
		metadata.Type = GetTypeFromFileExtension(filepath.extension());
			Ref<Asset> asset = AssetImporter::LoadAsset(handle, metadata);

			if (asset)
			{
				asset->Handle = handle;
				m_LoadedAssets[handle] = asset;
				m_AssetRegistry[handle] = metadata;
				SerializeAssetRegistry();
				return asset;
			}
		}
		catch (std::exception& e)
		{
			ENGINE_CORE_WARN("Unable to Import Asset {}\n{}", filepath.string(), e.what());
		}

		return nullptr;
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	const std::filesystem::path& EditorAssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetadata(handle).AssetPath;
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = Project::GetActiveAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "AssetRegistry";

			out << YAML::BeginSeq;
			{
				for (const auto& [handle, metadata] : m_AssetRegistry)
				{
					std::string filepathString = metadata.AssetPath.generic_string();

					out << YAML::BeginMap; // Asset
					out << YAML::Key << "Handle" << YAML::Value << handle;
					out << YAML::Key << "Filepath" << YAML::Value << filepathString;
					out << YAML::Key << "Type" << YAML::Value << Asset::AssetTypeToString(metadata.Type);
					out << YAML::EndMap; // Asset
				}
				out << YAML::EndSeq;
				out << YAML::EndMap; // AssetRegistry
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = Project::GetActiveAssetRegistryPath();

		YAML::Node data;

		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load project file {}. \n{}", path.string(), e.what());
			return false;
		}

		auto rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;

		for (const auto& node : rootNode)
		{
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry[handle];
			metadata.AssetPath = node["Filepath"].as<std::string>();
			metadata.Type = Asset::StringToAssetType(node["Type"].as<std::string>());
		}
		return true;
	}

}