/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

internal partial class HmPromptForm
{
    static public HmPromptForm form { get; set; }

    Timer timer = new Timer();
    IntPtr hWndHidemaru = IntPtr.Zero;
    public HmPromptForm(IntPtr hWndHidemaru, int consoleType, IntPtr hOutputPaneServer)
    {
        this.hWndOutputPaneServer = hOutputPaneServer;


        this.hWndHidemaru = hWndHidemaru;

        this.consoleType = (ConsoleType)consoleType;
        isClose = false;
        InitProcessAttr();
    }

    public void timer_Tick(object sender, EventArgs e)
    {
        this.timer_TickProcessWindow(sender, e);
    }

    private bool isClose = false;
    public bool IsClose()
    {
        return isClose;
    }
    public void Stop()
    {
        try
        {
            isClose = true;
            if (p != null)
            {
                p.Close();
                p.Kill();
                p = null;
            }
        }
        catch (Exception)
        {

        }
    }
}

