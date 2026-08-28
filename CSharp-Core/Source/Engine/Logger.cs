using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Engine
{
    public class Logger
    {
        public static void PrintString(string str)
        {
            unsafe { InternalCalls.PrintStringIcall(str); }
        }
    }
}
