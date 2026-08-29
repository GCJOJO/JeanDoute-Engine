#pragma once
#include "Engine/Scene/Node.h"
#include "Engine/Types/Vector.h"

namespace Engine
{
	class Node3D :
		public Node, 
		public AutoRegister<Node3D, Node3D, Node>
	{
		REGISTER_NODE_CLASS(Node3D);
	public:
		Transform3D& GetTransform() { return m_Transform; }

		virtual void OnReady() override { }
		virtual void OnUpdate(float dt) override { }
		virtual void OnDestroy() override { }

		void SetPosition(Vec3 position) { m_Transform.position = position; }
		void SetRotation(Vec3 rotation) { m_Transform.rotation = rotation; }
		void SetScale(Vec3 scale) { m_Transform.scale = scale; }
		void SetScale(float scale) { m_Transform.scale = scale; }


		virtual void SaveNode(YAML::Emitter& out) const override
		{
			Node::SaveNode(out);
			out << YAML::Key << "Transform" << YAML::BeginMap;
			out << YAML::Key << "Position" << YAML::Value << m_Transform.position;
			out << YAML::Key << "Rotation" << YAML::Value << m_Transform.rotation;
			out << YAML::Key << "Scale" << YAML::Value << m_Transform.scale;
			out << YAML::EndMap;
		}


		virtual void LoadNode(const YAML::Node& data) override
		{
			Node::LoadNode(data);
			const auto& transformData = data["Transform"];
			if (transformData)
			{
				m_Transform.position = transformData["Position"].as<Vec3>();
				m_Transform.rotation = transformData["Rotation"].as<Vec3>();
				m_Transform.scale = transformData["Scale"].as<Vec3>();
			}
		}

	protected:
		Transform3D m_Transform;
	};
}