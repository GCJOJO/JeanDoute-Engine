#include "enginepch.h"
#include "Engine/ScriptEngine/ScriptGlue.h"
#include "Engine/ScriptEngine/ScriptEngine.h"

#include "Coral/Assembly.hpp"

namespace Engine
{
#define ENGINE_ADD_INTERNAL_CALL(Name) assembly.AddInternalCall("Engine.InternalCalls", #Name, reinterpret_cast<void*>(&Name))

	static void ExitGame()
	{
		ScriptEngine::RequestStop();
	}

	static void PrintStringIcall(Coral::String InString)
	{
		ENGINE_INFO(std::string(InString));
	}

#pragma region Node

	static Coral::ManagedObject& Node_GetScriptInstance(UUID nodeId)
	{
		return ScriptEngine::GetManagedInstance(nodeId);
	}

	static UUID Node_FindByName(Coral::String name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ENGINE_CORE_ASSERT(scene);
		Node* node = scene->GetSceneTree().GetNode(std::string(name));

		if (!node)
			return 0;

		return node->GetId();
	}

	static void Node_SetName(UUID nodeId, Coral::String newName)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		ENGINE_CORE_ASSERT(scene);
		Node* node = scene->GetSceneTree().GetNode(nodeId);

		if (!node)
			return;

		node->SetName(std::string(newName));
	}

#pragma endregion Node

	void ScriptGlue::RegisterNodes()
	{

	}

	void ScriptGlue::RegisterFunctions(Coral::ManagedAssembly& assembly)
	{
		ENGINE_ADD_INTERNAL_CALL(ExitGame);
		ENGINE_ADD_INTERNAL_CALL(PrintStringIcall);
		
		ENGINE_ADD_INTERNAL_CALL(Node_GetScriptInstance);
		ENGINE_ADD_INTERNAL_CALL(Node_FindByName);
		ENGINE_ADD_INTERNAL_CALL(Node_SetName);

		assembly.UploadInternalCalls();
	}

}