using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Snuggler
{
    class Program
    {
        static void Main(string[] args)
        {
            Process pa;
            do
            {
                Thread.Sleep(1100);
                pa = Process.GetProcesses().FirstOrDefault(p => p.ProcessName.StartsWith("PinballArcade"));
            }
            while(pa == null || pa.MainWindowHandle == IntPtr.Zero);
            Console.WriteLine("{0}: 0x{1:x}", pa.ProcessName, pa.MainWindowHandle.ToInt64());
            Initialize(pa.MainWindowHandle, 176, 64);
            try
            {
                ulong tableId = 0;
                for(;;)
                {
                    try
                    {
                        Thread.Sleep(110);
                        ulong nextTableId = GetTableId(560, 16);
                        if(nextTableId == 0)
                        {
                            // The pinball application closed.
                            break;
                        }
                        if(tableId != nextTableId)
                        {
                            tableId = nextTableId;
                            Console.WriteLine(tableId);
                        }
                    }
                    catch
                    {
                        break;
                    }
                }
            }
            finally
            {
                Complete();
            }
        }

        // EXTERN_C SNUGGLING_API void Initialize(HWND window, int width, int height);
        [DllImport("Snuggling.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Initialize([In] IntPtr window, int width, int height);

        // EXTERN_C SNUGGLING_API table_id_t GetTableId(int x, int y);
        [DllImport("Snuggling.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong GetTableId(int x, int y);

        // EXTERN_C SNUGGLING_API void Complete();
        [DllImport("Snuggling.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Complete();
    }
}
