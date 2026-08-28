using Coral.Managed.Interop;

using System;
using System.Runtime.CompilerServices;

namespace Engine
{
#pragma warning disable 0649
    public static class InternalCalls
    {
        internal static unsafe delegate*<void> ExitGame;
        internal static unsafe delegate*<NativeString, void> PrintStringIcall;

        #region Node

        internal static unsafe delegate*<ulong, ref NativeInstance<dynamic>> Node_GetScriptInstance;
        internal static unsafe delegate*<NativeString, ulong> Node_FindByName;
        internal static unsafe delegate*<ulong, NativeString, void> Node_SetName;

        #endregion Node

        #region Scene

        internal static unsafe delegate*<ulong, void> SceneManager_OpenScene;
        //internal static unsafe delegate*<NativeString, ulong> Scene_CreateNode;
        //internal static unsafe delegate*<ulong, void> Scene_DestroyNode;

        #endregion

        /*
        #region TransformComponent

        internal static unsafe delegate*<ulong, out Vector3, void> TransformComponent_GetTranslation;
        internal static unsafe delegate*<ulong, ref Vector3, void> TransformComponent_SetTranslation;

        #endregion TransformComponent

        #region RigidBody2DComponent

        internal static unsafe delegate*<ulong, ref Vector2, ref Vector2, bool, void> RigidBody2DComponent_ApplyLinearImpulse;
        internal static unsafe delegate*<ulong, ref Vector2, bool, void> RigidBody2DComponent_ApplyLinearImpulseToCenter;
        internal static unsafe delegate*<ulong, out Vector2, void> RigidBody2DComponent_GetLinearVelocity;
        internal static unsafe delegate*<ulong, RigidBody2DComponent.BodyType> RigidBody2DComponent_GetType;
        internal static unsafe delegate*<ulong, RigidBody2DComponent.BodyType, void> RigidBody2DComponent_SetType;

        #endregion Rigidbody2DComponent

        #region SpriteRendererComponent

        internal static unsafe delegate*<ulong, out Color, void> SpriteRendererComponent_GetColor;
        internal static unsafe delegate*<ulong, ref Color, void> SpriteRendererComponent_SetColor;
        internal static unsafe delegate*<ulong, out float, void> SpriteRendererComponent_GetTilingFactor;
        internal static unsafe delegate*<ulong, ref float, void> SpriteRendererComponent_SetTilingFactor;

        #endregion SpriteRendererComponent

        #region CircleRendererComponent

        internal static unsafe delegate*<ulong, out Color, void> CircleRendererComponent_GetColor;
        internal static unsafe delegate*<ulong, ref Color, void> CircleRendererComponent_SetColor;
        internal static unsafe delegate*<ulong, out float, void> CircleRendererComponent_GetThickness;
        internal static unsafe delegate*<ulong, ref float, void> CircleRendererComponent_SetThickness;
        internal static unsafe delegate*<ulong, out float, void> CircleRendererComponent_GetFade;
        internal static unsafe delegate*<ulong, ref float, void> CircleRendererComponent_SetFade;

        #endregion CircleRendererComponent

        #region CameraComponent

        internal static unsafe delegate*<ulong, out bool, void> CameraComponent_GetPrimary;
        internal static unsafe delegate*<ulong, bool, void> CameraComponent_SetPrimary;
        internal static unsafe delegate*<ulong, out bool, void> CameraComponent_GetFixedAspectRatio;
        internal static unsafe delegate*<ulong, bool, void> CameraComponent_SetFixedAspectRatio;

        #endregion

        #region TextComponent

        internal static unsafe delegate*<ulong, out NativeString, void> TextComponent_GetText;
        internal static unsafe delegate*<ulong, NativeString, void> TextComponent_SetText;
        internal static unsafe delegate*<ulong, out Color, void> TextComponent_GetColor;
        internal static unsafe delegate*<ulong, ref Color, void> TextComponent_SetColor;

        #endregion
        */

        #region Input

        internal static unsafe delegate*<UInt16, bool> Input_IsKeyDown;

        #endregion Input

        #region Math

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static float Expression_Evaluate(string expression, float x);

        #endregion
    }
#pragma warning restore 0649
}
