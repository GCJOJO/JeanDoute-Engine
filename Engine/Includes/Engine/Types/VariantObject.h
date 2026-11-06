#pragma once
#include "Variant.h"
#include "EngineObject.h"
#include "Core/ObjectDB.h"

namespace Engine
{
	template<typename T> requires std::derived_from<T, EngineObject>
	struct VariantTraits<T*>
	{
		static constexpr VariantType Type = VariantType::Object;

		static void Write(Variant& variant, T* const& object)
		{
			const ObjectId id = object ? object->GetObjectId() : InvalidObjectId;
			std::memcpy(v.Raw(), &id, sizeof(id));
		}

		static void Read(const Variant& variant)
		{
			ObjectId id;
			std::memcpy(&id, v.Raw(), sizeof(id));
			return dynamic_cast<T*>(ObjectDB::Get(id));
		}
	};
}