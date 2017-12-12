/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class HmPromptFormProgram
{
    static void abc()
    {
        int nTickCount = 0;
        while(true)
        {
            System.Threading.Thread.Sleep(30);
            HmPromptForm.form.timer_Tick(null, null);
            nTickCount++;
            if (nTickCount % 10 == 0) { 
                if (!HidemaruWindowInfo.IsFindWindow())
                {
                    break;
                }
            }
        }
    }

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

        HmPromptForm.form = new HmPromptForm(hWndHidemaru, consoleType, hOutPane);
        System.Windows.Forms.MessageBox.Show("タスク卓性");
        Task task = new Task(abc);
        task.Start();
        Task.WaitAll(task);
    }
}

