#pragma once
#include <cstdint>

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
		Node,
	};

	class Variant 
	{
	public:
		template<typename T>
		T Get() 
		{

		}

		template<typename T>
		void Set(T) 
		{

		}

	private:
		VariantType m_Type;
		alignas(8) uint8_t m_Buffer;
	};
}