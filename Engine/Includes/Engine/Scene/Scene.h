#pragma once
#include "SceneTree.h"
#include "Engine/Core/Timestep.h"

namespace Engine
{
	class Scene
	{
	public:
		Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		void UpdateScene(Timestep ts, int viewId);

		SceneTree& GetSceneTree() { return m_SceneTree; }
	private:
		SceneTree m_SceneTree;
	};
}