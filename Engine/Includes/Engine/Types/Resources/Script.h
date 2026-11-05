#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Variant.h"
#include "Engine/Types/Resource.h"
#include "Engine/Types/StringName.h"
#include "Engine/Reflection/ClassInfo.h"
#include "Engine/ScriptEngine/ScriptLanguage.h"

namespace Engine
{
	class Node;
	class ScriptInstance;

	struct ScriptMethodInfo
	{
		StringName Name;
		VariantType ReturnType;
		std::vector<ArgInfo> Args;
	};

	struct ScriptPropertyInfo
	{
		StringName Name;
		VariantType Type;
		Variant DefaultValue;
	};

	struct ScriptMembers
	{
		std::vector<ScriptMethodInfo> Methods;
		std::vector<ScriptPropertyInfo> Properties;
		std::vector<SignalInfo> Signals;
	};

	class Script : public Resource
	{
	public:
		virtual ScriptLanguage* GetLanguage() const = 0;
		virtual StringName GetBaseClassName() const = 0;
		virtual bool CanInstantiate() const = 0;
		virtual Scope<ScriptInstance> CreateInstance(Node* owner) = 0;

		virtual const ScriptMembers& GetMembers() const = 0;
		virtual bool HasMethod(const StringName& method) = 0;
	};
}