#pragma once
#include "SceneTree.h"

namespace Engine
{
	class Scene
	{
	public:
		Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		SceneTree& GetSceneTree() { return m_SceneTree; }
	private:
		SceneTree m_SceneTree;
	};
}