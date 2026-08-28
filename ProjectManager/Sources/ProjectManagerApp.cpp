#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "ProjectManagerLayer.h"

namespace Engine
{

	class ProjectManagerApp : public Application
	{
	public:
		ProjectManagerApp(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new ProjectManagerLayer());
		}
		~ProjectManagerApp() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Editor - Project Manager";
		spec.CommandLineArgs = args;

		return new ProjectManagerApp(spec);
	}
}