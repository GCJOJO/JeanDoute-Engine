#pragma once

#include <glm/glm.hpp>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Engine
{
	struct Vec2
	{
		float x, y;

		Vec2(float value) : x(value), y(value) { }
		Vec2(float newX, float newY) : x(newX), y(newY) { }
		Vec2(const glm::vec2& vec) : x(vec.x), y(vec.y) { }

		operator glm::vec2() const { return glm::vec2(x, y); }
	};

	struct Vec3 
	{
		float x, y, z;

		Vec3() : x(0), y(0), z(0) { }
		Vec3(float value) : x(value), y(value), z(value) { }
		Vec3(float newX, float newY, float newZ) : x(newX), y(newY), z(newZ) { }
		Vec3(const glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z) { }

		operator glm::vec3() const { return { x, y, z }; }

		inline Vec3 operator+(const Vec3& other) const
		{
			Vec3 result = *this;
			result += other;
			return result;
		}
		inline Vec3 operator+(const int other) const
		{
			Vec3 result = *this;
			result += other;
			return result;
		}
		inline Vec3 operator+(const float other) const
		{
			Vec3 result = *this;
			result += other;
			return result;
		}

		inline Vec3 operator-(const Vec3& other) const
		{
			Vec3 result = *this;
			result -= other;
			return result;
		}
		inline Vec3 operator-(const int other) const
		{
			Vec3 result = *this;
			result -= other;
			return result;
		}
		inline Vec3 operator-(const float other) const
		{
			Vec3 result = *this;
			result -= other;
			return result;
		}

		inline Vec3 operator*(const Vec3& other) const
		{
			Vec3 result = *this;
			result *= other;
			return result;
		}
		inline Vec3 operator*(const int other) const
		{
			Vec3 result = *this;
			result *= other;
			return result;
		}
		inline Vec3 operator*(const float other) const
		{
			Vec3 result = *this;
			result *= other;
			return result;
		}

		inline Vec3 operator/(const Vec3& other) const
		{
			Vec3 result = *this;
			result /= other;
			return result;
		}
		inline Vec3 operator/(const int other) const
		{
			Vec3 result = *this;
			result /= other;
			return result;
		}
		inline Vec3 operator/(const float other) const
		{
			Vec3 result = *this;
			result /= other;
			return result;
		}

		Vec3& operator+=(const Vec3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		Vec3& operator+=(const int& value)
		{
			x += value;
			y += value;
			z += value;
			return *this;
		}
		Vec3& operator+=(const float& value)
		{
			x += value;
			y += value;
			z += value;
			return *this;
		}

		Vec3& operator-=(const Vec3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}
		Vec3& operator-=(const int& value)
		{
			x -= value;
			y -= value;
			z -= value;
			return *this;
		}
		Vec3& operator-=(const float& value)
		{
			x -= value;
			y -= value;
			z -= value;
			return *this;
		}

		Vec3& operator*=(const Vec3& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}
		Vec3& operator*=(const int& factor)
		{ 
			x *= factor;
			y *= factor;
			z *= factor;
			return *this;
		}
		Vec3& operator*=(const float& factor)
		{ 
			x *= factor;
			y *= factor;
			z *= factor;
			return *this;
		}

		Vec3& operator/=(const Vec3& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
		}
		Vec3& operator/=(const int& factor)
		{
			x /= factor;
			y /= factor;
			z /= factor;
			return *this;
		}
		Vec3& operator/=(const float& factor)
		{
			x /= factor;
			y /= factor;
			z /= factor;
			return *this;
		}

		static const Vec3 UP;
		static const Vec3 DOWN;
		static const Vec3 FORWARD;
		static const Vec3 BACKWARD;
		static const Vec3 LEFT;
		static const Vec3 RIGHT;
	};

	struct Vec4
	{
		float x, y, z, w;

		Vec4(float value) : x(value), y(value), z(value), w(value) {}
		Vec4(float newX, float newY, float newZ, float newW) : x(newX), y(newY), z(newZ), w(newW) {}
		Vec4(const glm::vec4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

		operator glm::vec4() const { return {x, y, z, w}; }
	};

	struct Transform2D
	{
		Vec2 position = Vec2(0.0f);
		float angle = 0.0f;
		Vec2 scale = Vec2(1);

		const glm::mat3& ToMatrix();
	};

	struct Transform3D 
	{
		Vec3 position = Vec3(0.0f);
		Vec3 rotation = Vec3(0.0f);
		Vec3 scale = Vec3(1.0f);

		const glm::mat4& ToMatrix();
	};

	inline std::ostream& operator<<(std::ostream& os, const Vec3& vec)
	{
		return os << vec.x << ", " << vec.y << ", " << vec.z;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
}

namespace std
{
	template <>
	struct std::formatter<Engine::Vec3> : std::formatter<std::string> {
		auto format(Engine::Vec3 v, format_context& ctx) const {
			return formatter<string>::format(
				std::format("{}, {}, {}", v.x, v.y, v.z), 
				ctx);
		}
	};
}

template <> struct fmt::formatter<Engine::Vec3> : fmt::ostream_formatter {};

namespace YAML
{
	template<>
	struct convert<Engine::Vec3>
	{
		static Node encode(const Engine::Vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Engine::Vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}