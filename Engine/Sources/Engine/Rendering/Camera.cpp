#include "enginepch.h"
#include "Engine/Rendering/Camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <bx/math.h>

namespace Engine {

	float* Camera::GetBxMatrix()
	{
		return glm::value_ptr(m_Projection);
	}

}