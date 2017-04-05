using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace HmBaloonToast
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    /// 
    public partial class App : Application
    {
        static public List<string> args = new List<string>();
        static public string message = "";
        static public int time = 5000;

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            foreach (string arg in e.Args)
            {
                args.Add(arg);
            }

            if (args.Count >= 1)
            {
                message = e.Args[0];
            }

            if (args.Count >= 2)
            {
                int t = 0;
                bool success = int.TryParse(e.Args[1], out t);
                if (success)
                {
                    time = t;
                }
            }
        }
    }
}
