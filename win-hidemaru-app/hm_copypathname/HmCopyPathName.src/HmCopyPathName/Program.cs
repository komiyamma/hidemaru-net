/*
 * Copyright (c) 2015-2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace HmCopyPathName
{
    class Program
    {
        [DllImport(@"C:\usr\hidemaru\HmSharedOutputPane.dll")]
        private extern static void SetSharedMessageW(String msg);

        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                return;
            }

            var path = args[0];
            Clipboard.SetText(path);

            try
            {
                SetSharedMessageW(path);
            }
            catch (Exception)
            {

            }
        }
    }
}
