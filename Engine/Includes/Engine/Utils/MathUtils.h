#pragma once

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
	};
}