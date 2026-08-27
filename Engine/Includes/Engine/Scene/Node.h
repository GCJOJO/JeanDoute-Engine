#pragma once
#include "Engine/Core/UUID.h"
#include "Engine/Types/StringName.h"

namespace Engine
{
	class Node
	{
	public:
		Node();
		Node(StringName name);
		virtual ~Node();

		void Ready();
		void Update(float dt);
		void Free();

		virtual void OnReady(){ }
		virtual void OnUpdate(float dt) { }
		virtual void OnDestroy() { }

		const UUID& GetId() const { return m_Id; }
		const StringName GetName() const { return m_Name; }

		void SetName(StringName newName);

		Node* AddChild(Scope<Node> newChild);
		std::vector<Node*> GetChildren(bool recursive = false);
		Node* GetChild(uint32_t index);
		const Node* GetChild(uint32_t index) const;
		Node* GetChild(StringName childName);
		void RemoveChild(Node* child);

	private:
		UUID m_Id;
		UUID m_ParentId{ UUID::EmptyHandle() };
		std::vector<Scope<Node>> m_Children{ };
		std::unordered_map<StringName, Node*> m_ChildrenNames{ };

		StringName m_Name = "Node";
	};
}