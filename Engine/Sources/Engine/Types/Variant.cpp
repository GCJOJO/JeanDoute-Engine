#include "enginepch.h"
#include "Engine/Types/Variant.h"
#include "Engine/Types/EngineObject.h"

namespace Engine
{
	const char* VariantTypeName(VariantType type)
	{
		static constexpr const char* names[] =
		{
			"None",
			"Float", "Double",
			"Bool", "Char", "Byte", "Short", "Int", "Long",
			"UByte", "UShort", "UInt", "ULong",
			"String",
			"Vector2", "Vector3", "Vector4", "Color",
			"Object",
			"Node"
		};

		static_assert(std::size(names) == static_cast<std::size_t>(VariantType::Count));
		return names[static_cast<std::size_t>(type)];
	}

	Variant Variant::MakeObject(ObjectId id)
	{
		Variant v;
		std::memcpy(v.Raw(), &id, sizeof(id));
		v.m_Type = VariantType::Object;
		return v;
	}

	ObjectId Variant::GetObjectId() const
	{
		ENGINE_CORE_ASSERT(m_Type == VariantType::Object, std::format("Variant must be of type Object, is {} instead", VariantTypeName(m_Type)));
		ObjectId id;
		std::memcpy(&id, m_Buffer, sizeof(id));
		return id;
	}

	bool Variant::CanConvert(VariantType from, VariantType to)
	{
		if (from == to) return true;
		switch (from)
		{
		case VariantType::Bool:		return to == VariantType::Int;
		case VariantType::Int:		return to == VariantType::Float || to == VariantType::Double;
		case VariantType::Float:	return to == VariantType::Int;
		default:					return false;
		}
	}

	Variant Variant::ConvertTo(VariantType to) const
	{
		if (m_Type == to) return *this;
		ENGINE_CORE_ASSERT(CanConvert(m_Type, to), std::format("Cannot Convert Variant Type {} to {}", VariantTypeName(m_Type), VariantTypeName(to)));

		switch (m_Type) 
		{
		case VariantType::Bool:				return Variant(static_cast<std::int64_t>(Get<bool>()));
		case VariantType::Int:
			if (to == VariantType::Float)	return Variant(static_cast<double>(Get<std::int64_t>()));
			return Variant(Get<std::int64_t>() != 0);
		case VariantType::Float:			return Variant(static_cast<std::int64_t>(Get<double>()));
		default:							return Variant();
		}
	}

	bool Variant::operator==(const Variant& other) const
	{
		return m_Type == other.m_Type && std::memcmp(m_Buffer, other.m_Buffer, BufferSize);
	}

	std::string Variant::ToString()
	{
		switch (m_Type)
		{
		case VariantType::None:   return "None";
		case VariantType::Bool:   return Get<bool>() ? "true" : "false";
		case VariantType::Int:    return std::to_string(Get<std::int64_t>());
		case VariantType::Float:  return std::to_string(Get<double>());
		case VariantType::String: return "\"" + Get<StringName>().ToString() + "\"";
		case VariantType::Vector2: { auto v = Get<glm::vec2>(); return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
		case VariantType::Vector3: { auto v = Get<glm::vec3>(); return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"; }
		case VariantType::Vector4: { auto v = Get<glm::vec4>(); return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"; }
		case VariantType::Color: { auto v = Get<glm::vec4>(); return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"; }
		case VariantType::Object: return "Object(#" + std::to_string(GetObjectId()) + ")";
		default:                  return "?";
		}
	}

}