#pragma once

namespace Coral
{
	class ManagedAssembly;
}

namespace Engine
{

	class ScriptGlue
	{
	public:
		static void RegisterNodes();
		static void RegisterFunctions(Coral::ManagedAssembly& assembly);
	};
}