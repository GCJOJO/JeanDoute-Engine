#include "enginepch.h"
#include "Engine/Types/Vector.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Engine
{
	const Vec3 Vec3::UP			{  0,  1,  0 };
	const Vec3 Vec3::DOWN		{  0, -1,  0 };
	const Vec3 Vec3::FORWARD	{  0,  0, -1 };
	const Vec3 Vec3::BACKWARD	{  0,  0,  1 };
	const Vec3 Vec3::LEFT		{ -1,  0,  0 };
	const Vec3 Vec3::RIGHT		{  1,  0,  0 };

	const glm::mat3& Transform2D::ToMatrix()
	{
		return glm::translate(glm::mat3(1.0f), (glm::vec2)position) *
			glm::rotate(glm::mat3(1.0f), glm::radians(angle))*
			glm::scale(glm::mat3(1.0f), (glm::vec2)scale);
	}

	const glm::mat4& Transform3D::ToMatrix()
	{
		glm::vec3 rotationRadians = glm::vec3(
			glm::radians(rotation.x),
			glm::radians(rotation.y),
			glm::radians(rotation.z));

		glm::mat4 rotation = glm::toMat4(glm::quat(rotationRadians));

		return glm::translate(glm::mat4(1.0f), glm::vec3(position)) *
			rotation *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	}


}