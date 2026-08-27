#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Assets/Asset.h"

namespace Engine
{
	class Scene;

	class SceneManager 
	{
	public:
		static void Initialize(Ref<SceneManager> sceneManager)
		{
			m_SceneManager = sceneManager;
		}

		static void NewScene() { m_SceneManager->NewSceneImpl(); }
		static void OpenScene(AssetHandle handle) { m_SceneManager->OpenSceneImpl(handle); }
		static void OpenScene(const std::filesystem::path& scenePath) { m_SceneManager->OpenSceneImpl(scenePath); }
		static Ref<Scene> GetCurrentScene() { return m_SceneManager->GetCurrentSceneImpl(); }

	protected:
		virtual void NewSceneImpl() = 0;
		virtual void OpenSceneImpl(AssetHandle handle) = 0;
		virtual void OpenSceneImpl(const std::filesystem::path& scenePath) = 0;
		virtual Ref<Scene> GetCurrentSceneImpl() = 0;

	private:
		static Ref<SceneManager> m_SceneManager;
	};
}