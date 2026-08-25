#pragma once
#include "Engine/Core/Layer.h"

namespace Engine
{
	class EditorLayer : public Layer 
	{
	public:
		void OnAttach() override;
		void OnDetach() override;


		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;


		void OnEvent(Event& event) override;
	};
}