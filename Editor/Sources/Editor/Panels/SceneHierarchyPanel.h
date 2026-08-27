#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawNode(Node* node);

	private:
		Ref<Scene> m_Context = nullptr;
		Node* m_SelectionContext = nullptr;
	};
}


