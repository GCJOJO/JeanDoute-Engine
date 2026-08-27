#pragma once
#include "Engine/Scene/Node.h"
#include "Engine/Types/Vector.h"

namespace Engine
{
	class Node3D :
		public Node, 
		public AutoRegister<Node3D, Node3D, Node>
	{
	public:
		Transform3D& GetTransform() { return m_Transform; }

		virtual void OnReady() override { }
		virtual void OnUpdate(float dt) override { }
		virtual void OnDestroy() override { }

		void SetPosition(Vec3 position) { m_Transform.position = position; }
		void SetRotation(Vec3 rotation) { m_Transform.rotation = rotation; }
		void SetScale(Vec3 scale) { m_Transform.scale = scale; }
		void SetScale(float scale) { m_Transform.scale = scale; }

	protected:
		Transform3D m_Transform;
	};
}