#pragma once
#include "Engine/Types/StringName.h"
#include "Engine/Core/Variant.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <map>

namespace Engine
{
	class EngineObject;

	using Invoker = Variant(*)(void* self, Variant* args);
	using PropertyGet = Variant(*)(void* self);
	using PropertySet = void(*)(void* self, const Variant& value);
	using ClassFactory = EngineObject*(*)();

	using MetaInfo = std::unordered_map<StringName, std::string>;

	struct ArgInfo 
	{
		StringName Name;
		VariantType Type;
		std::optional<Variant> DefaultValue;
	};

	struct MethodInfo
	{
		StringName Name;
		VariantType ReturnType;
		std::vector<ArgInfo> Args;
		bool IsConst;
		bool IsStatic;
		bool IsPure;
		Invoker Invoker = nullptr;
		MetaInfo Meta;
	};

	struct PropertyInfo
	{
		StringName Name;
		VariantType Type;
		PropertyGet Getter = nullptr;
		PropertySet Setter = nullptr;
		MetaInfo Meta;
	};

	struct SignalInfo
	{
		StringName Name;
		std::vector<ArgInfo> Args;
	};

	using MethodMap = std::map<StringName, MethodInfo>;
	using PropertyMap = std::map<StringName, PropertyInfo>;
	using SignalMap = std::map<StringName, SignalInfo>;

	struct ClassInfo 
	{
		StringName Name;
		StringName ParentName;
		const ClassInfo* ParentInfo;
		ClassFactory Factory = nullptr;
		MethodMap Methods;
		PropertyMap Properties;
		SignalMap Signals;

		const MethodInfo* FindMethod(StringName methodName) const 
		{
			auto it = Methods.find(methodName);
			if (it != Methods.end())
				return &it->second;

			if (ParentInfo != nullptr)
				return ParentInfo->FindMethod(methodName);

			return nullptr;
		}

		const PropertyInfo* FindProperty(StringName propertyName) const 
		{
			auto it = Properties.find(propertyName);
			if (it != Properties.end())
				return &it->second;

			if (ParentInfo != nullptr)
				return ParentInfo->FindProperty(propertyName);

			return nullptr;
		}
	};
}