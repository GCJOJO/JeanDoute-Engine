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

		Ref<Scene> GetEditorScene() const { return m_EditorScene; }

	protected:
		void NewSceneImpl() override;
		void OpenSceneImpl(AssetHandle handle) override;
		void OpenSceneImpl(const std::filesystem::path& scenePath) override;
		Ref<Scene> GetCurrentSceneImpl() override;

	private:
		Ref<Scene> m_ActiveScene, m_EditorScene;
	};
}



