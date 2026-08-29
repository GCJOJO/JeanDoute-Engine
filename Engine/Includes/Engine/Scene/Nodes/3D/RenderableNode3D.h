#pragma once
#include "Engine/Scene/Nodes/Node3D.h"

#include "bgfx/bgfx.h"

namespace Engine
{
	class RenderableNode3D : 
		public Node3D, 
		public AutoRegister<RenderableNode3D, RenderableNode3D, Node3D, Node>
	{
		REGISTER_NODE_CLASS(RenderableNode3D);
	public:
		virtual void Render(const bgfx::ViewId& viewId) { }

		virtual void OnReady() override { }
		virtual void OnUpdate(float dt) override { }
		virtual void OnDestroy() override { }
	};
}