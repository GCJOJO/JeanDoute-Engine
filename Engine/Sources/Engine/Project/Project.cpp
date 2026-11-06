#include "enginepch.h"
#include "Engine/Project/Project.h"
#include "Engine/Project/ProjectSerializer.h"

namespace Engine
{
	Project::Project()
	{

	}

	std::filesystem::path Project::GetAssetAbsolutePath(const std::filesystem::path& path)
	{
		return GetAssetDirectory() / path;
	}

	SharedPtr<Project> Project::New()
	{
		s_ActiveProject = CreateSharedPtr<Project>();
		return s_ActiveProject;
	}

	SharedPtr<Project> Project::Load(const std::filesystem::path& path)
	{
		SharedPtr<Project> project = CreateSharedPtr<Project>();
		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;
			std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
			s_ActiveProject->m_AssetManager = editorAssetManager;
			editorAssetManager->DeserializeAssetRegistry();
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}
		return false;
	}

}