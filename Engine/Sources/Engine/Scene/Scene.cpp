#include "enginepch.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	Scene::Scene()
	{
		m_SceneTree.SetRoot(CreateScope<Node>("Root", m_SceneTree));
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		return newScene;
	}

}