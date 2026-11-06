#pragma once
#include "Engine/Scene/SceneManager.h"

#include <filesystem>

namespace Engine
{
	class EditorSceneManager : public SceneManager
	{
	public:
		EditorSceneManager();

		void OnScenePlay();
		void OnSceneStop();

		SharedPtr<Scene> GetEditorScene() const { return m_EditorScene; }

	protected:
		void NewSceneImpl() override;
		void OpenSceneImpl(AssetHandle handle) override;
		void OpenSceneImpl(const std::filesystem::path& scenePath) override;
		SharedPtr<Scene> GetCurrentSceneImpl() override;

	private:
		SharedPtr<Scene> m_ActiveScene, m_EditorScene;
	};
}



