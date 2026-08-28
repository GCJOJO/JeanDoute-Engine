using System;

namespace Engine
{
    public class Node
    {
        protected Node() 
        { 
            ID = 0;
        }

        internal Node(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;
        public unsafe string Name 
        {
            set
            {
                Name = value;
                InternalCalls.Node_SetName(ID, value);
            }
        }

        public virtual void Ready()
        {

        }

        public virtual void Process(float deltaTime)
        {

        }

        public unsafe Node? FindNodeByName(string name)
        {
            ulong entityID = InternalCalls.Node_FindByName(name);
            Logger.PrintString($"Entity {name} found with id {entityID}");
            if (entityID == 0)
                return null;
            return new Node(entityID);
        }

        public unsafe T? As<T>() where T : Node, new()
        {
            Coral.Managed.Interop.NativeInstance<dynamic> nativeInstance = InternalCalls.Node_GetScriptInstance(ID);
            Node? instance = nativeInstance.Get();

            if (instance == null)
            {
                //Logger.PrintString("Instance is null !");
                return null;
            }
            //Logger.PrintString("Instance is not null");

            if (instance is not T)
            {
                //Logger.PrintString($"Instance is not of the type {typeof(T).FullName}");
                return null;
            }
            //Logger.PrintString("Instance is correct type");
            return instance as T;
        }
    }
}
