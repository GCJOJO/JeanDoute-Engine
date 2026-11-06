#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Nodes/3D/RenderableNode3D.h"

namespace Engine
{
	Scene::Scene()
	{
		m_SceneTree.SetRoot(CreateScope<Node>("Root", m_SceneTree));
	}

	SharedPtr<Scene> Scene::Copy(SharedPtr<Scene> other)
	{
		SharedPtr<Scene> newScene = CreateSharedPtr<Scene>();
		return newScene;
	}

	void Scene::StartScene()
	{
		for (auto& node : NodeRegistry<Node>::GetAll())
		{
			node->Ready();
		}
	}

	void Scene::UpdateScene(Timestep ts, int viewId)
	{
		for (auto& node : NodeRegistry<RenderableNode3D>::GetAll())
			node->Render(viewId);
	}

	void Scene::StopScene()
	{

	}

}