#pragma once
#include "Node.h"

namespace Engine
{
	class SceneTree
	{
	public:
		static SceneTree& Get();

		SceneTree();

		Node* GetRoot() { return m_Root.get(); }
		void SetRoot(Scope<Node> newRoot);
		void ClearSceneTree();

		void RegisterNode(Node* node);
		void UnregisterNode(Node* node);
		Node* GetNode(UUID id);

	private:
		Scope<Node> m_Root;
		std::unordered_map<UUID, Node*> m_Nodes;
	};
}