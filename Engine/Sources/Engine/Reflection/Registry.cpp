#include "enginepch.h"
#include "Engine/Reflection/Registry.h"

#include "Engine/Types/RefCounted.h"

#include "Engine/Scene/Node.h"
#include "Engine/Scene/Nodes/Node3D.h"
#include "Engine/Scene/Nodes/3D/RenderableNode3D.h"
#include "Engine/Scene/Nodes/3D/MeshNode3D.h"

namespace Engine
{
	const ClassInfo* Registry::Find(StringName className)
	{
		auto it = Classes().find(className);
		return it != Classes().end() ? &it->second : nullptr;
	}

	const ClassInfo& Registry::Get(StringName className)
	{
		const ClassInfo* classInfo = Find(className);
		ENGINE_CORE_ASSERT(classInfo, std::format("Registry: Cannot get class {}, unknown class", className));
		return *classInfo;
	}

	EngineObject* Registry::Create(StringName className)
	{
		const ClassInfo* info = Find(className);
		return (info && info->Factory) ? info->Factory() : nullptr;
	}

	bool Registry::IsSubclassOf(StringName child, StringName parent)
	{
		for (const ClassInfo* classInfo = Find(child); classInfo; classInfo = classInfo->ParentInfo)
			if (classInfo->Name == parent)
				return true;
		return false;
	}

	std::vector<StringName> Registry::GetDerivedClasses(StringName className)
	{
		std::vector<StringName> result{ };

		for (const auto& [name, info] : Classes())
		{
			if (name == className)
				continue;

			for (const ClassInfo* classInfo = info.ParentInfo; classInfo; classInfo = classInfo->ParentInfo)
			{
				if (classInfo->Name == className)
				{
					result.push_back(name);
					break;
				}
			}
		}

		return result;
	}

	std::string Registry::ExportJson()
	{
		return "";
	}

	void Registry::RegisterEngineClasses()
	{
		Register<EngineObject>();
		Register<RefCounted>();
		//Register<Resource>();
		//Register<Script>();

		Register<Node>();
		Register<Node3D>();
		Register<RenderableNode3D>();
		Register<MeshNode3D>();
	}

}