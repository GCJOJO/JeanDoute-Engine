#pragma once
#include "Engine/Core/UUID.h"
#include "Engine/Types/StringName.h"
#include "NodeRegistry.h"

#include <concepts>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include "yaml-cpp/exceptions.h"

namespace Engine
{
	class SceneTree;

	class Node : public AutoRegister<Node, Node>
	{
	public:
		Node();
		Node(StringName name);
		Node(StringName name, SceneTree& sceneTree);
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

		template<typename T, typename ... Args>
		requires DerivedFromNode<T>
		T* CreateChild(Args&& ...args)
		{
			return static_cast<T*>(AddChild(CreateScope<T>(std::forward<Args>(args)...)));
		}

		Node* AddChild(Scope<Node> newChild);
		std::vector<Node*> GetChildren(bool recursive = false);
		Node* GetChild(uint32_t index);
		const Node* GetChild(uint32_t index) const;
		Node* GetChild(StringName childName);
		void RemoveChild(Node* child);

		void SaveNode(YAML::Emitter& out) const;

		void SetScript(StringName script) { m_ScriptName = script; }
		StringName GetScriptName() const { return m_ScriptName; }

	protected:
		UUID m_Id;
		UUID m_ParentId{ UUID::EmptyHandle() };
		std::vector<Scope<Node>> m_Children{ };
		std::unordered_map<StringName, Node*> m_ChildrenNames{ };

		StringName m_Name = "Node";
		StringName m_ScriptName = "";
	};

	template<typename T>
	concept DerivedFromNode = std::derived_from<T, Node>;
}