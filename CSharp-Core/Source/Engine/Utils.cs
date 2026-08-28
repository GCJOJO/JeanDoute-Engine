using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Engine
{
    public class Utils
    {
        public unsafe static void ExitGame() { InternalCalls.ExitGame(); }

        private static bool TimersActive = true;

        [UnmanagedCallersOnly]
        public static void SetTimersActive() { TimersActive = true; }

        [UnmanagedCallersOnly]
        public static void SetTimersInactive() { TimersActive = false; }

        public static async void DelayAction(int millisecond, Action action)
        {
            await Task.Delay(millisecond);
            if(TimersActive)
                action?.Invoke();
        }
    }
}
