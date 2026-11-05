#pragma once
#include "Engine/Core/Core.h"
#include "ClassInfo.h"
#include "Engine/Types/StringName.h"
#include "Engine/Types/EngineObject.h"

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
			if (Classes().contains(className))
				return;

			ClassInfo info;
			info.Name = className;

			if constexpr (!std::is_void_v<typename T::Super>)
			{
				const StringName parentClassName = T::Super::StaticClassName();
				const ClassInfo* parentClassInfo = Find(parentClassName);
				ENGINE_CORE_ASSERT(parent, "Registry : Register parent class before child class");
				info.ParentName = parentClassName;
				info.ParentInfo = parentClassInfo;
			}

			if constexpr (std::is_default_constructible_v<T> && !std::is_abstract_v<T>)
				info.Factory = []() -> EngineObject* { return new T(); };

			ClassInfo& stored = Classes().emplace(className, std::move(info)).first->second;

			ClassBuilder<T> builder(stored);

			if constexpr (requires { T::Bind(builder); })
				T::Bind(builder);
		}

		static const ClassInfo* Find(StringName className);
		static const ClassInfo& Get(StringName className);

		static EngineObject* Create(StringName className);
		static bool IsSubclassOf(StringName child, StringName parent);
		static std::vector<StringName> GetDerivedClasses(StringName className);

		static std::string ExportJson();

		static void RegisterEngineClasses();
	private:
		static std::map<StringName, ClassInfo, std::less<>>& Classes()
		{
			static std::map<StringName, ClassInfo, std::less<>> classes;
			return classes;
		}
	};
}