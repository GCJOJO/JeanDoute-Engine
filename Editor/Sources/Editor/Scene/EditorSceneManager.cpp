#include "EditorSceneManager.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{

	EditorSceneManager::EditorSceneManager()
	{
		m_EditorScene = CreateSharedPtr<Scene>();
		m_ActiveScene = m_EditorScene;
	}

	void EditorSceneManager::OnScenePlay()
	{
		m_ActiveScene = Scene::Copy(m_EditorScene);
	}

	void EditorSceneManager::OnSceneStop()
	{
		m_ActiveScene = m_EditorScene;
	}

	void EditorSceneManager::NewSceneImpl()
	{
		m_EditorScene = CreateSharedPtr<Scene>();
		m_ActiveScene = m_EditorScene;
	}

	void EditorSceneManager::OpenSceneImpl(AssetHandle handle)
	{

	}

	void EditorSceneManager::OpenSceneImpl(const std::filesystem::path& scenePath)
	{

	}

	SharedPtr<Scene> EditorSceneManager::GetCurrentSceneImpl()
	{
		return m_ActiveScene;
	}
}