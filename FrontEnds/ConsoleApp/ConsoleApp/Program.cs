using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace SDRAlsaConsole
{
    class Program
    {
        [DllImport("E:\\Projects\\SDRAlsaClient\\trunk\\SDRAlsaClient\\bin\\Debug\\SDRAlsaClient.dll")]
        private static extern int RunClient();
        [DllImport("E:\\Projects\\SDRAlsaClient\\trunk\\SDRAlsaClient\\bin\\Debug\\SDRAlsaClient.dll")]
        private static extern IntPtr AppLastError();

        static void Main(string[] args)
        {
            int r = RunClient();
            if (r==-1)
            {
                string s = Marshal.PtrToStringAnsi(AppLastError());
                Console.WriteLine("Error: {0}", s);
            }
            Console.WriteLine("Exiting...");
        }
    }
}
