#pragma once

namespace Engine
{
	template <typename R, typename C, bool Const, bool Static, typename... Args>
	struct MethodTraitsBase
	{
		using ClassType = C;
		using ReturnType = R:
		using ArgsTuple = std::tuple<std::remove_cvref_t<Args>...>;
		using RawArgs = std::tuple<Args...>;

		static constexpr size_t ArgsCount = sizeof...(Args);
		static constexpr bool IsConst = Const;
		static constexpr bool IsStatic = Static;

		template<size_t I> using Arg = std::tuple_element_t<I, ArgsTuple>;
	};

	
	template <typename T> struct MethodTraits;

	template <typename R, typename C, typename... Args>
	struct MethodTraits<R (C::*)(Args...)> : MethodTraitsBase<R, C, false, false, Args...> { };

	template <typename R, typename C, typename... Args>
	struct MethodTraits<R(C::*)(Args...) const> : MethodTraitsBase<R, C, true, false, Args...> { };

	template <typename R, typename... Args>
	struct MethodTraits<R(*)(Args...)> : MethodTraitsBase<R, C, false, true, Args...> { };

	template <typename R, typename C, typename... Args>
	struct MethodTraits<R(C::*)(Args...) noexcept> : MethodTraitsBase<R, C, false, false, Args...> {};

	template <typename R, typename C, typename... Args>
	struct MethodTraits<R(C::*)(Args...) const noexcept> : MethodTraitsBase<R, C, true, false, Args...> {};

	template <typename R, typename... Args>
	struct MethodTraits<R(*)(Args...) noexcept> : MethodTraitsBase<R, C, false, true, Args...> {};
}