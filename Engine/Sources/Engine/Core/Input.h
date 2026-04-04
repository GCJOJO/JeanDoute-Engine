#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

namespace Engine 
{
	class ENGINE_API Input 
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		
		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}