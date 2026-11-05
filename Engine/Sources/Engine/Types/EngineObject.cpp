#include "enginepch.h"
#include "Engine/Types/EngineObject.h"

namespace Engine
{
	EngineObject::EngineObject()
	{
		// Register to ClassDB
		Registry::Register()
	}


	EngineObject::~EngineObject()
	{
		// Unregister to ClassDB
		Registry::Unregister();
	}

	bool EngineObject::IsClass(std::string_view className) const
	{
		return IsClass(StringName(className));
	}

	bool EngineObject::IsClass(StringName className) const
	{
		for (const ClassInfo* classInfo = &GetClassInfo(); classInfo; classInfo = classInfo->ParentInfo)
			if (classInfo->Name == className)
				return true;
		return false;
	}

	Variant EngineObject::Call(StringName method, Variant* args, std::size_t argCount)
	{
		const MethodInfo* methodInfo = GetClassInfo().FindMethod(method);
		if (!methodInfo || methodInfo->Args.size() != argCount)
			return Variant();
		return methodInfo->Invoker(static_cast<void*>(this), args);
	}

	bool EngineObject::Get(StringName property, Variant& out) const
	{
		const PropertyInfo* propertyInfo = GetClassInfo().FindProperty(property);
		if (!propertyInfo)
			return false;
		out = propertyInfo->Getter(const_cast<EngineObject*>(this));
		return true;
	}

	bool EngineObject::Set(StringName property, const Variant& value)
	{
		const PropertyInfo* propertyInfo = GetClassInfo().FindProperty(property);
		if (!propertyInfo)
			return false;
		propertyInfo->Setter(const_cast<EngineObject*>(this), value);
		return true;
	}

	void EngineObject::GetPropertyList(std::vector<const PropertyInfo*>& out) const
	{

	}

}