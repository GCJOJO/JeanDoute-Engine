#pragma once
#include "Engine/Core/Core.h"
#include "ClassInfo.h"
#include "Engine/Types/StringName.h"

namespace Engine
{
	class Registry
	{
	public:
		Registry() = delete;

		template <typename T>
		static void Register()
		{
			StringName className = T::StaticClassName();
		}

		static const ClassInfo& Find(StringName className);
		static const ClassInfo& Get(StringName className);

		static Ref<EngineObject> Create(StringName className);
		static bool IsSubclassOf(StringName child, StringName parent);
		static std::vector<StringName> GetSubclasses(StringName className);

		static std::string ExportJson();


	private:
		static std::map<StringName, ClassInfo, std::less<>>& Classes()
		{
			static std::map<StringName, ClassInfo, std::less<>> classes;
			return classes;
		}
	};
}