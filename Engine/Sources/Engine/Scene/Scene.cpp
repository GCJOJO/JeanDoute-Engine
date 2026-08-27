#include "enginepch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Nodes/3D/RenderableNode3D.h"

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

	void Scene::UpdateScene(Timestep ts, int viewId)
	{
		for (auto& node : Renderable3DRegistry::Get().GetAll())
			node->Render(viewId);
	}

}