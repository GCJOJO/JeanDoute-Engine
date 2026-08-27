#include "enginepch.h"
#include "Engine/Assets/SceneImporter.h"
#include "Engine/Scene/SceneSerializer.h"

#include "Engine/Project/Project.h"

namespace Engine
{
	Ref<Scene> SceneImporter::LoadScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		ENGINE_PROFILE_FUNCTION();

		return ImportScene(Project::GetActiveAssetDirectory() / metadata.AssetPath);
	}

	Ref<Scene> SceneImporter::ImportScene(const std::filesystem::path& path)
	{
		ENGINE_PROFILE_FUNCTION();

		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
		return scene;
	}

	void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		ENGINE_PROFILE_FUNCTION();

		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

}