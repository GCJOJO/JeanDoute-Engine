#pragma once

#include <math.h>

#define PI 3.14159265358979323846264338327950288
constexpr double pi_over_180 = PI / 180;
constexpr double inv_over_pi = 180 / PI;

namespace Engine
{
	class MathUtils
	{
	public:
		static inline float DegToRad(float deg)
		{
			return deg * pi_over_180;
		}

		static inline float RadToDeg(float rad)
		{
			return rad * inv_over_pi;
		}

		static inline float cos(float value)
		{
			return std::cos(value);
		}

		static inline float cosh(float value)
		{
			return std::cosh(value);
		}

		static inline float sin(float value)
		{
			return std::sin(DegToRad(value));
		}

		static inline float sinh(float value)
		{
			return std::sinh(DegToRad(value));
		}

		static inline float tan(float value)
		{
			return std::tan(DegToRad(value));
		}

		static inline float tanh(float value)
		{
			return std::tanh(DegToRad(value));
		}
	};
}