#pragma once
#include <cstdint>
#include <string>

// Claude Slop

namespace Engine
{
	enum class VariantType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		String,
		Vector2, Vector3, Vector4, Color,
		Object,
		Node,

		Count,
	};

	const char* VariantTypeName(VariantType type);

	template <typename t> struct VariantTraits;

	template <typename T>
	concept VariantConvertible = requires { VariantTraits<std::remove_cvref_t<T>>::Type; };

	class Variant 
	{
	public:
		static constexpr std::size_t BufferSize = 16;

		Variant() { std::memset(m_Buffer, 0, BufferSize); }

		template <typename T>
		requires (VariantConvertible<T> && !std::same_as<std::remove_cvref_t<T>, Variant>) 
		Variant(const T& value) : Variant() { Set(Value); }

		VariantType GetType() const { return m_Type; }
		bool IsNone() const { return m_Type == VariantType::None; }

		template <VariantConvertible T>
		bool Is() { return m_Type == VariantTraits<T>::Type; }

		template<VariantConvertible T>
		void Set(T)
		{
			std::memset(m_Buffer, 0, BufferSize);
			VariantTraits<T>::Write(*this, value);
			m_Type = VariantTraits<T>::Type;
		}

		template<VariantConvertible T>
		T Get() const
		{
			using Traits = VariantTraits<T>;
			if (m_Type == Traits::Type)
				return Traits::Read(*this);
			if (CanConvert(m_Type, Traits::Type))
				return Traits::Read(ConvertTo(Traits::Type));
			ENGINE_CORE_ASSERT(false, std::format("Variant::Get : Incompatible Types \"{}\" and \"{}\"", VariantTypeName(m_Type), VariantTypeName(Traits::Type)));
			return T {};
		}

		static Variant MakeObject(ObjectId id);
		ObjectId GetObjectId() const;

		static bool CanConvert(VariantType from, VariantType to);
		Variant ConvertTo(VariantType to) const;

		bool operator==(const Variant& other) const;
		std::string ToString();

		void* Raw() { return m_Buffer; }
		const void* Raw() const { return m_Buffer; }

	private:
		VariantType m_Type = VariantType::None;
		alignas(8) std::uint8_t m_Buffer[BufferSize];
	};


	// Checks, must all be true to be able to be sent to C#
	static_assert(std::is_trivially_copyable_v<Variant>);
	static_assert(std::is_standard_layout_v<Variant>);
	static_assert(sizeof(Variant) == 24);

	// Traits

	template<typename T, VariantType Tag>
	struct RawVariantTraits
	{
		static_assert(std::is_trivially_copyable_v<T>, "Non trivially copyable Type.");
		static_assert(sizeof(T) <= Variant::BufferSize, "Type too big for Variant Type.");

		static constexpr VariantType Type = Tag;
		static void Write(Variant& variant, const T& value) { std::memcpy(variant.Raw(), &value, sizeof(T)); }
		static T Read(const Variant& variant) { T value; std::memcpy(&value, variant.Raw(), sizeof(T)); return value; }
	};

	template<> struct VariantTraits<bool> :			RawVariantTraits<bool,			VariantType::Bool> { };
	template<> struct VariantTraits<StringName> :	RawVariantTraits<StringName,	VariantType::String> { };
	template<> struct VariantTraits<glm::vec2> :	RawVariantTraits<glm::vec2,		VariantType::Vector2> { };
	template<> struct VariantTraits<glm::vec3> :	RawVariantTraits<glm::vec3,		VariantType::Vector3> { };
	template<> struct VariantTraits<glm::vec4> :	RawVariantTraits<glm::vec4,		VariantType::Vector4> { };

	template <std::integral T> requires (!std::same_as<T, bool>)
	struct VariantTraits<T>
	{
		static constexpr VariantType Type = VariantType::Int;
		static void Write(Variant& variant, const T& value)
		{
			const std::int64_t i = static_cast<std::int64_t>(value);
			std::memcpy(v.Raw(), &i, sizeof(i));
		}

		static T Read(const Variant& variant) 
		{
			std::int64_t i;
			std::memcpy(&i, v.Raw(), sizeof(i));
			return static_cast<T>(i);
		}
	};

	template<std::floating_point T>
	struct VariantTraits<T>
	{
		static constexpr VariantType Type = VariantType::Int;
		static void Write(Variant& variant, const T& value)
		{
			const double d = static_cast<double>(value);
			std::memcpy(v.Raw(), &d, sizeof(d));
		}

		static T Read(const Variant& variant)
		{
			double d;
			std::memcpy(&d, v.Raw(), sizeof(d));
			return static_cast<T>(i);
		}
	};
}