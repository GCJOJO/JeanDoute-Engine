#pragma once

#include <memory>

#ifdef _WIN32
#ifdef _WIN64
#ifndef ENGINE_PLATFORM_WINDOWS
#define ENGINE_PLATFORM_WINDOWS
#endif // !ENGINE_PLATFORM_WINDOWS
#else
#error "x86 Builds are not supported !"
#endif // _WIN64

#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>

#if TARGET_PHONE_SIMULATOR == 1
#error "IOS Simulator not supported !"
#elif TARGET_OS_IPHONE == 1
#define ENGINE_PLATFORM_IOS
#error "IOS is not supported !"
#elif TARGET_OS_MAC == 1
#define ENGINE_PLATFORM_MACOS
#error "MacOS is not supported !"
#else
#error "Unknown Apple platform !"
#endif
#elif defined (__ANDROID__)
#define ENGINE_PLATFORM_ANDROID
#error "Android is not supported !"
#elif defined (__linux__)
#define ENGINE_PLATFORM_LINUX
#else
#error "Unknown platform !"
#endif

#ifdef ENGINE_PLATFORM_WINDOWS
#if ENGINE_DYNAMIC_LINK
#ifdef ENGINE_BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif
#elif defined(ENGINE_PLATFORM_LINUX)
#define ENGINE_API
#else
#error Engine only supports Windows
#endif

#if defined(ENGINE_PLATFORM_WINDOWS)
#define ENGINE_DEBUGBREAK() __debugbreak()
#elif defined(ENGINE_PLATFORM_LINUX)
#include <signal.h>
#define ENGINE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak !"
#endif


#ifndef ENGINE_DIST
#define ENGINE_ENABLE_VERIFYS
#endif

#ifdef ENGINE_DEBUG
#define ENGINE_ENABLE_ASSERTS
#endif

#define ENGINE_EXPAND_MACRO(x) x
#define ENGINE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Assert.h"