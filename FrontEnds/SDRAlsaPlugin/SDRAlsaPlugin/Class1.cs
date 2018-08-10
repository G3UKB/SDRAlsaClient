using System;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

using SDRSharp.Common;
using SDRAlsaPlugin;

namespace SDRSharp.SDRAlsaPlugin
{
    public class SDRAlsaPlugin : ISharpPlugin
    {
        private const string _displayName = "SDRAlsa";
        private ISharpControl _controlInterface;
        private UserControl1 _panel;
        private FreqProcessor _freqProcessor;

        [DllImport("E:\\Projects\\SDRAlsaClient\\trunk\\SDRAlsaClient\\bin\\Debug\\SDRAlsaClient.dll")]
        private static extern int RunClient();
        [DllImport("E:\\Projects\\SDRAlsaClient\\trunk\\SDRAlsaClient\\bin\\Debug\\SDRAlsaClient.dll")]
        private static extern IntPtr AppLastError();

        public UserControl Gui
        {
            get { return _panel; }
        }

        public string DisplayName
        {
            get { return _displayName; }
        }

        public void Close()
        {
        }

        public void Initialize(ISharpControl control)
        {
            _controlInterface = control;
            _panel = new UserControl1(_controlInterface);

            /*
            int r = RunClient();
            if (r == -1)
            {
                string s = Marshal.PtrToStringAnsi(AppLastError());
                //Console.WriteLine("Error: {0}", s);
                MessageBox.Show(s, "Error", MessageBoxButtons.OK);
            }
            */
            _freqProcessor = new FreqProcessor(_controlInterface);
        }
    }
}
