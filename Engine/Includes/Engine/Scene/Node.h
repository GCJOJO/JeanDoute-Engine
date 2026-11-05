#pragma once
#include "NodeFactory.h"
#include "NodeRegistry.h"
#include "Engine/Core/UUID.h"
#include "Engine/Types/StringName.h"
#include "Engine/Types/EngineObject.h"
#include "Engine/Types/Resources/Script.h"

#include <concepts>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include "yaml-cpp/exceptions.h"

namespace Engine
{

#define REGISTER_NODE_CLASS(ClassName)												\
public:																				\
	static StringName get_static_class_name() { return #ClassName; } 				\
    virtual StringName get_class_name() const override { return #ClassName; }		\
																					\
private:																			\
    static inline bool ClassName##_registered = NodeRegistrar<ClassName>::registered

	class SceneTree;

	class Node : public EngineObject, public AutoRegister<Node, Node>
	{
		ENGINE_CLASS(Node, EngineObject)
	public:
		Node();
		Node(StringName name);
		Node(StringName name, SceneTree& sceneTree);
		virtual ~Node();

		virtual StringName get_class_name() const { return {"Node"}; }

		void SetScript(Ref<Script> script);
		Ref<Script> GetScript() const { return m_Script; }

		Variant Call(StringName methodName, Variant* args, std::size_t argCount) override;
		bool	Get(StringName propertyName, Variant& out) const override;
		bool	Set(StringName propertyName, const Variant& value) override;
		void	GetPropertyList(std::vector<const PropertyInfo*>& out) const override;

		void Notification(int what);

		void Ready();
		void Update(float dt);
		void Free();

		virtual void OnReady(){ }
		virtual void OnUpdate(float dt) { }
		virtual void OnDestroy() { }

		const UUID& GetId() const { return m_Id; }
		const UUID& GetParentId() const { return m_ParentId; }
		Node* GetParent() const;
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

		virtual void SaveNode(YAML::Emitter& out) const;
		virtual void LoadNode(const YAML::Node& data);

		void SetScript(StringName script) { m_ScriptName = script; }
		StringName GetScriptName() const { return m_ScriptName; }

	protected:
		UUID m_Id;
		UUID m_ParentId{ UUID::EmptyHandle() };
		std::vector<Scope<Node>> m_Children{ };
		std::unordered_map<StringName, Node*> m_ChildrenNames{ };

		StringName m_Name = "Node";
		StringName m_ScriptName = "";
	private:
		Ref<Script>				m_Script;
		Scope<ScriptInstance>	m_ScriptInstance = nullptr;
	};

	template<typename T>
	concept DerivedFromNode = std::derived_from<T, Node>;

	static inline const bool NodeRegistered = [] {							
		NodeFactory::Get().RegisterNodeClass(StringName("Node"), [] {		
			return new Node();												
			});																
		return true;														
		}();
}