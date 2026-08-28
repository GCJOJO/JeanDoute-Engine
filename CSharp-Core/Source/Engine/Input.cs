
using System;

namespace Engine
{
    public class Input
    {
        public unsafe static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown((UInt16)keycode);
        }
    }
}