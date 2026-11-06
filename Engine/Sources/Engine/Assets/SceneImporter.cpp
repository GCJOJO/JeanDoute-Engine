#include "enginepch.h"
#include "Engine/Assets/SceneImporter.h"
#include "Engine/Scene/SceneSerializer.h"

#include "Engine/Project/Project.h"

namespace Engine
{
	SharedPtr<Scene> SceneImporter::LoadScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		ENGINE_PROFILE_FUNCTION();

		return ImportScene(Project::GetActiveAssetDirectory() / metadata.AssetPath);
	}

	SharedPtr<Scene> SceneImporter::ImportScene(const std::filesystem::path& path)
	{
		ENGINE_PROFILE_FUNCTION();

		SharedPtr<Scene> scene = CreateSharedPtr<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
		return scene;
	}

	void SceneImporter::SaveScene(SharedPtr<Scene> scene, const std::filesystem::path& path)
	{
		ENGINE_PROFILE_FUNCTION();

		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

}