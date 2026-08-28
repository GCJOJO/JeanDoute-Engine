using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Engine
{
    public class SceneManager
    {
        public static void OpenScene(ulong sceneId)
        {
            unsafe { InternalCalls.SceneManager_OpenScene(sceneId); }
        }
    }
}
