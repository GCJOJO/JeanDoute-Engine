#include "Engine.h"
#include "Engine/Core/EntryPoint.h"
#include "Engine/Core/Application.h"
#include "EditorLayer.h"

namespace Engine
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}
		~EditorApp() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = " Editor";
		spec.CommandLineArgs = args;

		return new EditorApp(spec);
	}
}