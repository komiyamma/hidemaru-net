using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class HmPromptFormProgram
{
    [STAThread]
    static void Main(string[] args)
    {

        IntPtr hWndHidemaru = new IntPtr(Int64.Parse(args[0]));
        int consoleType = Int32.Parse(args[1]);
        IntPtr hOutPane = new IntPtr(Int64.Parse(args[2]));

        HmPromptForm.ResetStyleChange();
        HmPromptForm.WriteSharedMemory(hWndHidemaru);

        if (System.Diagnostics.Process.GetProcessesByName(
            System.Diagnostics.Process.GetCurrentProcess().ProcessName).Length > 1)
        {
            //すでに起動していると判断する
            return;
        }

        HmPromptForm f = new HmPromptForm(hWndHidemaru, consoleType, hOutPane);
        f.ShowDialog();
    }
}

