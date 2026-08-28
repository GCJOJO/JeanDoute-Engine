#pragma once
#include "Node.h"

namespace Engine
{
	class SceneTree
	{
	public:
		static SceneTree& Get();

		SceneTree() { };

		Node* GetRoot() { return m_Root.get(); }
		void SetRoot(Scope<Node> newRoot);
		void ClearSceneTree();

		void RegisterNode(Node* node);
		void UnregisterNode(Node* node);
		Node* GetNode(UUID id);
		Node* GetNode(StringName name);
		Node* GetNode(const std::string& name);

		template<DerivedFromNode T>
		T* GetNode(UUID id)
		{
			return static_cast<T>(GetNode(id));
		}

		template<DerivedFromNode T>
		T* GetNode(StringName name)
		{
			return static_cast<T>(GetNode(name));
		}

		template<DerivedFromNode T>
		T* GetNode(const std::string& name)
		{
			return static_cast<T>(GetNode(name));
		}

	private:
		Scope<Node> m_Root;
		std::unordered_map<UUID, Node*> m_Nodes;
		std::unordered_map<StringName, Node*> m_NodeNames;
	};
}