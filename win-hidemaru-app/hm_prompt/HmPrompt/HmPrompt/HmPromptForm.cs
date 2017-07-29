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

    public HmPromptForm(IntPtr command)
    {
        this.command = (Command)command;
        isClose = false;
        InitProcessAttr();

        timer.Interval = 30;
        timer.Enabled = true;
        timer.Start();
        timer.Tick += timer_Tick;
    }

    private void timer_Tick(object sender, EventArgs e)
    {
        this.timer_TickProcessWindow(sender, e);
    }

    private bool isClose = false;
    public bool IsClose()
    {
        return isClose;
    }
    public void Close()
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

