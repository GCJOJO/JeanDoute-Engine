#include "enginepch.h"
#include "Engine/Scene/SceneTree.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	SceneTree& SceneTree::Get()
	{
		return SceneManager::GetCurrentScene()->GetSceneTree();
	}

	void SceneTree::SetRoot(Scope<Node> newRoot)
	{
		m_Root = std::move(newRoot);
	}

	void SceneTree::ClearSceneTree()
	{
		for (const auto& node : m_Nodes)
		{
			node.second->Free();
		}

		m_Nodes.clear();
	}

	void SceneTree::RegisterNode(Node* node)
	{
		m_Nodes[node->GetId()] = node;
	}

	void SceneTree::UnregisterNode(Node* node)
	{
		m_Nodes.erase(node->GetId());
	}

	Node* SceneTree::GetNode(UUID id)
	{
		auto it = m_Nodes.find(id);
		return (it != m_Nodes.end()) ? it->second : nullptr;
	}

}