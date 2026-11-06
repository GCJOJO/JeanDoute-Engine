#pragma once
#include <compare>
#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>


#include "Engine/Types/RefCounted.h"

namespace Engine
{
	// Claude Slop

	template <typename T>
	class RefPtr
	{
	public:
		RefPtr() = default;
		RefPtr(std::nullptr_t) { }
		explicit RefPtr(T* ptr) : m_Ptr(ptr) { Acquire(); }

		RefPtr(const RefPtr& other) : m_Ptr(other.m_Ptr) { Acquire(); }
		RefPtr(RefPtr&& other) noexcept : m_Ptr(std::exchange(other.m_Ptr, nullptr)) {  }

		// Derived Class to Base Class cast
		template<typename U> requires std::convertible_to<U*, T*>
		RefPtr(const RefPtr<U>& other) : m_Ptr(other.m_Ptr) { Acquire(); }

		template<typename U> requires std::convertible_to<U*, T*>
		RefPtr(RefPtr<U>&& other) noexcept : m_Ptr(std::exchange(other.m_Ptr, nullptr)) {}

		~RefPtr() { Release(); }

		// Affectation : Copy and swap
		RefPtr& operator=(const RefPtr& other) { RefPtr(other).Swap(*this); return *this; }
		RefPtr& operator=(RefPtr&& other) noexcept { RefPtr(std::move(other)).Swap(*this); return *this; }
		RefPtr& operator=(std::nullptr_t) { Reset(); return *this; }

		// Getters
		T* Get() const { return m_Ptr; }
		T* operator->() const { return m_Ptr; }
		T& operator*() const { return *m_Ptr; }

		explicit operator bool() const { return m_Ptr != nullptr; }
		bool IsValid() const { return m_Ptr != nullptr; }

		void Reset() { RefPtr().Swap(*this); }
		void Swap(RefPtr& other) noexcept { std::swap(m_Ptr, other.m_Ptr); }

		// Cast to a derived type
		template <typename U>
		RefPtr<U> Cast() const { return RefPtr<U>(dynamic_cast<U*>(m_Ptr)); }

		friend bool operator==(const RefPtr& a, const RefPtr& b) { return a.m_Ptr == b.m_Ptr; }
		friend bool operator==(const RefPtr& a, std::nullptr_t) { return a.m_Ptr == nullptr; }
		friend auto operator<=>(const RefPtr& a, const RefPtr& b)
		{
			return std::compare_three_way{}(a.m_Ptr, b.m_Ptr);
		}

	private:
		template<typename U> friend class RefPtr;

		void Acquire() const 
		{
			static_assert(std::is_base_of_v<RefCounted, T>, "RefPtr<T> : Class must be derived from RefCounted");
			if (m_Ptr)
				m_Ptr->Reference();
		}

		void Release() const 
		{
			static_assert(std::is_base_of_v<RefCounted, T>, "RefPtr<T> : Class must be derived from RefCounted");
			if (m_Ptr && m_Ptr->Unreference())
				delete m_Ptr;
		}

	private:
		T* m_Ptr = nullptr;
	};

	template<typename T, typename... Args>
	RefPtr<T> CreateRefPtr(Args&&... args)
	{
		return RefPtr<T>(new T(std::forward(args)...));
	}
}

template<typename T>
struct std::hash<Engine::RefPtr<T>>
{
	struct size_t operator()(const Engine::RefPtr<T>& ref) const noexcept
	{
		return std::hash<T*>{}(ref.Get());
	}
};