#include "enginepch.h"
#include "Engine/Scene/Node.h"
#include "Engine/Scene/SceneTree.h"

namespace Engine
{

	Node::Node()
	{
		m_Name = "Node";
		SceneTree::Get().RegisterNode(this);
	}

	Node::Node(StringName name)
		: m_Name(name)
	{
		SceneTree::Get().RegisterNode(this);
	}

	Node::Node(StringName name, SceneTree& sceneTree)
	{
		m_Name = name;
		sceneTree.RegisterNode(this);
	}

	Node::~Node()
	{
		SceneTree::Get().UnregisterNode(this);
	}

	void Node::Ready()
	{
		OnReady();
		for (const auto& child : m_Children)
			child->OnReady();
	}

	void Node::Update(float dt)
	{
		OnUpdate(dt);
		for (const auto& child : m_Children)
			child->OnUpdate(dt);
	}

	void Node::Free()
	{
		OnDestroy();
		//Free resources
	}

	void Node::SetName(StringName newName)
	{
		if (m_ParentId)
		{
			Node* parent = SceneTree::Get().GetNode(m_ParentId);
			if (parent)
			{
				parent->m_ChildrenNames.erase(m_Name);
				parent->m_ChildrenNames[newName] = this;
			}
		} 

		m_Name = newName;
	}
	
	Node* Node::AddChild(Scope<Node> newChild)
	{
		int i = 1;
		StringName oldName = newChild->GetName();
		while (GetChild(newChild->GetName()))
		{
			i++;
			std::string newName = std::format("{} {}", oldName, i);
			newChild->SetName(newName);
		}

		newChild->m_ParentId = this->GetId();
		Node* childPtr = newChild.get();
		m_ChildrenNames[newChild->GetName()] = childPtr;
		m_Children.push_back(std::move(newChild));
		return childPtr;
	}

	std::vector<Node*> Node::GetChildren(bool recursive)
	{
		std::vector<Node*> children{ };
		for (const auto& child : m_Children)
		{
			children.push_back(child.get());
			if (recursive)
			{
				const auto& recursiveChildren = child->GetChildren();
				children.insert(std::end(children), std::begin(recursiveChildren), std::end(recursiveChildren));
			}
		}
		
		return children;
	}

	Node* Node::GetChild(uint32_t index)
	{
		if (index >= m_Children.size())
			return nullptr;
		return m_Children[index].get();
	}

	const Node* Node::GetChild(uint32_t index) const
	{
		if (index >= m_Children.size())
			return nullptr;
		return m_Children[index].get();
	}

	Node* Node::GetChild(StringName childName)
	{
		if (m_ChildrenNames.empty())
			return nullptr;

		auto it = m_ChildrenNames.find(childName);
		return (it != m_ChildrenNames.end()) ? it->second : nullptr;
	}

	void Node::RemoveChild(Node* child)
	{
		m_ChildrenNames.erase(child->GetName());
		m_Children.erase(std::remove_if(m_Children.begin(), m_Children.end(),
			[child](const auto& c) { return c->GetId() == child->GetId(); }),
			m_Children.end());
	}

	void Node::SaveNode(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Id" << YAML::Value << GetId();
		//out << YAML::Key << "Type" << YAML::Value << 
		out << YAML::Key << "Name" << YAML::Value << GetName().ToString();

		out << YAML::EndMap;
	}

}