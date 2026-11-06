#pragma once
#include "EngineObject.h"

namespace Engine
{
	class RefCounted : public EngineObject
	{
	public:
		RefCounted() = default;
		virtual ~RefCounted() = default;

		void Reference()	{ m_RefCounted.fetch_add(1, std::memory_order_relaxed); }
		bool Unreference()	{ return m_RefCounted.fetch_sub(1, std::memory_order_acq_rel) == 1; }

		std::int32_t GetReferenceCount() const { return m_RefCounted.load(std::memory_order_relaxed); }
	private:
		std::atomic<std::int32_t> m_RefCounted{ 0 };
	};
}