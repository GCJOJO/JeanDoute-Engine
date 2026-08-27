#pragma once
#include "Node.h"

namespace Engine
{
	class SceneTree
	{
	public:
		static SceneTree& Get();

		void ClearSceneTree();

		void RegisterNode(Node* node);
		void UnregisterNode(Node* node);
		Node* GetNode(UUID id);
	private:
		std::unordered_map<UUID, Node*> m_Nodes;
	};
}