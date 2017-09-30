using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Snuggler
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Dictionary<ulong, BitmapImage> codeToImageMap = new Dictionary<ulong, BitmapImage>();
        private Dictionary<ulong, string> codeToPathMap = new Dictionary<ulong, string>();
        private Timer timer = new Timer(110) { AutoReset = true };
        private Process currentProcess;
        private ulong currentTableId;

        public MainWindow()
        {
            // Read the mapping of table codes to image paths.
            var strings = System.IO.File.ReadAllLines(@"C:\Users\Chris\Documents\CodesToPaths.txt");
            var q = from s in strings
                    let p = s.Split(new[] { '\t', ' ', ',' }, StringSplitOptions.RemoveEmptyEntries)
                    where p.Length >= 2 && p[0].All(c => Char.IsDigit(c))
                    let k = UInt64.Parse(p[0])
                    select new KeyValuePair<ulong, string>(k, p[1]);
            codeToPathMap = q.ToDictionary(p => p.Key, p => p.Value);

            // Initialize this window.
            InitializeComponent();

            // Start a timer to detect Pinball Arcade state changes.
            timer.Elapsed += Timer_Elapsed;
            timer.Enabled = true;
        }

        private void Timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            try
            {
                // Stop the timer.  This method is not re-entrant.
                timer.Enabled = false;

                // Update the process.
                var process = Process.GetProcesses().FirstOrDefault(p => p.ProcessName.StartsWith("PinballArcade"));
                if(process != null)
                {
                    if(currentProcess == null)
                    {
                        currentProcess = process;
                        Initialize();
                    }
                }
                else if(currentProcess != null)
                {
                    currentProcess = null;
                    Complete();
                }

                // If Pinball Arcade is running, display an appropriate image.
                if(currentProcess != null)
                {
                    ulong tableId = GetTableId(8, 256, 64);
                    if(currentTableId != tableId)
                    {
                        currentTableId = tableId;
                        if(currentTableId != 0)
                        {
                            // If there isn't a bitmap for this table, load one.
                            if(!codeToImageMap.TryGetValue(currentTableId, out BitmapImage bitmapImage))
                            {
                                // Get its path, if any.
                                if(!codeToPathMap.TryGetValue(currentTableId, out string path))
                                {
                                    // There isn't one; don't change the view.
                                    return;
                                }

                                Dispatcher.Invoke(() =>
                                {
                                    // Create a bitmap image from the file and add it to the map.
                                    bitmapImage = new BitmapImage();
                                    bitmapImage.BeginInit();
                                    try
                                    {
                                        bitmapImage.UriSource = new Uri(path);
                                    }
                                    finally
                                    {
                                        bitmapImage.EndInit();
                                    }
                                });
                                codeToImageMap.Add(currentTableId, bitmapImage);
                            }

                            // Set the image on the main thread.
                            Dispatcher.Invoke(() => image.Source = bitmapImage);
                        }
                    }
                }
                else
                {
                    Dispatcher.Invoke(() => image.Source = null);
                }
            }
            finally
            {
                // Restart the timer.
                timer.Enabled = true;
            }
        }

        // EXTERN_C SNUGGLING_API void Initialize();
        [DllImport("Snuggling.dll", CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void Initialize();

        // EXTERN_C SNUGGLING_API table_id_t GetTableId(int y, int width, int height);
        [DllImport("Snuggling.dll", CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern ulong GetTableId(int y, int width, int height);

        // EXTERN_C SNUGGLING_API void Complete();
        [DllImport("Snuggling.dll", CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void Complete();
    }
}
