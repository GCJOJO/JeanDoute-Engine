#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Core/Layer.h>
#include "Config.h"

namespace Engine
{
	class ProjectManagerLayer : public Layer
	{
	public:
		ProjectManagerLayer();
		virtual ~ProjectManagerLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:

		void NewProject();
		bool OpenProject();
		bool OpenProject(const std::filesystem::path& path);

		bool HasPremakeInstalled();
		bool InstallPremake();

	private:
		Ref<ProjectManagerConfig> m_Config;
	};
}