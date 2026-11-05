#pragma once

namespace Engine 
{
	template <typename T>
	class VariantTrait
	{
	public:
		VariantType GetType();
		Variant ToVariant(T);
		T FromVariant(Variant);
	};
}