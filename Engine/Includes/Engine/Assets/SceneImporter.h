#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Assets/AssetMetadata.h"

#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static Ref<Scene> LoadScene(AssetHandle handle, const AssetMetadata& metadata);

		// Load from filepath
		static Ref<Scene> ImportScene(const std::filesystem::path& path);

		static void SaveScene(Ref<Scene> scene, const std::filesystem::path& path);
	};
}