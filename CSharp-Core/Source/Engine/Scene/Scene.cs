using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Engine
{
    public class Scene
    {
        public unsafe static Node CreateEntity(String name)
        {
            /*ulong entityId = InternalCalls.Scene_CreateEntity(name);
            Node entity = new Node(entityId);
            return entity;*/
            return null;
        }

        public unsafe static void DestroyEntity(ref Node entity)
        {
            //InternalCalls.Scene_DestroyEntity(entity.ID);
            //entity = null;
        }
    }
}
