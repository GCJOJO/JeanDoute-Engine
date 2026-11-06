#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const SharedPtr<Scene>& context);

		void SetContext(const SharedPtr<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawNode(Node* node);

	private:
		SharedPtr<Scene> m_Context = nullptr;
		Node* m_SelectionContext = nullptr;
	};
}


