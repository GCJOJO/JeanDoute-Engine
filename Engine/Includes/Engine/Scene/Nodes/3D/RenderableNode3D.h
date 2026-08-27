#pragma once
#include "Engine/Scene/Nodes/Node3D.h"

#include "bgfx/bgfx.h"

namespace Engine
{

	//VERY TEMP !!!
	class RenderableNode3D;

	class Renderable3DRegistry
	{
	public:
		static Renderable3DRegistry& Get()
		{
			static Renderable3DRegistry registry;
			return registry;
		}

		void Register(RenderableNode3D* node)
		{
			m_Renderables.push_back(node);
		}

		void Unregister(RenderableNode3D* node)
		{
			std::erase(m_Renderables, node);
		}

		std::vector<RenderableNode3D*>& GetAll() { return m_Renderables; }

	private:
		std::vector<RenderableNode3D*> m_Renderables{ };
	};

	class RenderableNode3D : public Node3D
	{
	public:
		RenderableNode3D() { Renderable3DRegistry::Get().Register(this); }
		virtual ~RenderableNode3D() { Renderable3DRegistry::Get().Unregister(this); }

		virtual void Render(const bgfx::ViewId& viewId) { }

		virtual void OnReady() override { }
		virtual void OnUpdate(float dt) override { }
		virtual void OnDestroy() override { }
	};
}