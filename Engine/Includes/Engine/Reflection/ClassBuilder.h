#pragma once

#include <type_traits>
#include <tuple>

#include "Engine/Core/Variant.h"
#include "MethodTraits.h"
#include "ClassInfo.h"
#include "ClassInfo.h"

namespace Engine
{
	template<typename Tuple> struct AllConvertible;

	template<typename... Ts>
	struct AllConvertible<std::tuple<Ts...>>
	{
		static constexpr bool value = (VariantConvertible<Ts> && ...);
	};


	// Constraints
	template<typename T, typename C>
	concept MemberOfOrBase = std::is_void_v<C> || std::is_base_of_v<C, T>;

	template<typename Traits>
	concept AllVariantConvertible = AllConvertible<typename Traits::ArgsTuple>::value
		&& (std::is_void_v<typename Traits::ReturnType> || VariantConvertible<typename Traits::ReturnType>);

	template<typename T, auto Fn>
	concept RegisterableMethod = MemberOfOrBase<T, typename MethodTraits<decltype(Fn)>::ClassType>
		&& AllVariantConvertible<MethodTraits<decltype(Fn)>>;

	// Class Builder
	template <typename T>
	class ClassBuilder
	{
	public:
		explicit ClassBuilder(ClassInfo& classInfo) : m_Info(classInfo) {}

		template <typename Fn>
			requires RegisterableMethod<T, Fn>
		ClassBuilder& Method<Fn>(StringName name, std::vector<StringName> argsNames = {}) 
		{
			return *this;
		}

		template <typename Fn>
			requires RegisterableMethod<T, Fn>
		ClassBuilder& StaticMethod<Fn>(StringName name, std::vector<StringName> argsNames = {})
		{
			return *this;
		}

		template <auto Getter, auto Setter = nullptr>
			requires RegisterableMethod<T, Fn>
		ClassBuilder& Property(StringName name)
		{
			return *this;
		}

		ClassBuilder& Signal(StringName name, std::vector<ArgInfo> argsName = {})
		{
			return *this;
		}

		ClassBuilder& Meta(StringName methodName, StringName key, std::string value);
		ClassBuilder& Default(StringName methodName, StringName argName, Variant value);
	private:
		template<auto Fn>
		static MethodInfo MakeMethodInfo(StringName name, std::vector<StringName> argNames);

		template<auto Fn>
		static Variant InvokeImpl(void* self, Variant* args);

		template<auto Fn, std::size_t... I>
		static Variant InvokeImplIndexed(void* self, Variant* args, std::index_sequence<I...>);

		void AddMethod(MethodInfo&& info);
		void AddProperty(PropertyInfo&& info);

		ClassInfo& m_Info;
	};
}