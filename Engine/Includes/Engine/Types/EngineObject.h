#pragma once

#include "Engine/Reflection/Registry.h"
#include "Engine/Reflection/ClassInfo.h"
#include "Engine/Reflection/ClassBuilder.h"
#include <cstdint>

namespace Engine
{
	using ObjectId = uint64_t;
	constexpr ObjectId InvalidObjectId = 0;

	struct Callable
	{
		ObjectId Target = InvalidObjectId;
		StringName Method;
	};

#define ENGINE_CLASS(Self, SuperClass)														\
	public:																					\
		using Super = SuperClass;															\
		static const char* StaticClassMeta() { return #Self; }								\
		const ClassInfo& GetClassMeta() const override										\
		{																					\
			static const ClassInfo* info = Registry::Get(#Self);							\
			return *info;																	\
		}	

	class EngineObject
	{
		ENGINE_CLASS(EngineObject, nullptr);
	public:
		EngineObject();
		~EngineObject();

		EngineObject(const EngineObject&) = delete;
		EngineObject& operator=(const EngineObject&) = delete;

		ObjectId GetObjectId() const { return m_Id; }

		virtual const ClassInfo& GetClassMeta() const = 0;
		const StringName GetClassName() const { return GetClassMeta().Name; }
		bool IsClass(std::string_view className) const;
		bool IsClass(StringName className) const;

		virtual Variant		Call(StringName method, Variant* args, std::size_t argCount);
		virtual bool		Get(StringName property, Variant& out) const;
		virtual bool		Set(StringName property, const Variant& value);
		virtual void		GetPropertyList(std::vector<const PropertyInfo*>& out) const;
		
		static void Bind(ClassBuilder<EngineObject>& builder) { }
	private:
		ObjectId m_Id;
		uint32_t m_Flags;
		std::map<StringName, std::vector<Callable>>* m_Signals = nullptr;
	};
}