#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"

namespace Engine
{

	SceneHierarchyPanel::SceneHierarchyPanel(const SharedPtr<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const SharedPtr<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = nullptr;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			DrawNode(m_Context->GetSceneTree().GetRoot());
		}

		ImGui::End();

		ImGui::Begin("Properties");

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawNode(Node* node)
	{
		//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == node) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)node, flags, (const char*)node->GetName());
		if (ImGui::IsItemClicked())
			m_SelectionContext = node;
		
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiDockNodeFlags_PassthruCentralNode;
			for (const auto& child : node->GetChildren())
				DrawNode(child);
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (m_SelectionContext == node)
				m_SelectionContext = nullptr;
			//node->Free();
		}
	}

}