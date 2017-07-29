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

    public HmPromptForm(String command, String arguments, Boolean isSettingMode)
    {
        this.promptCommand = command;
        this.promptArguments = arguments;
        this.isSettingMode = isSettingMode;
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
                try
                {
                    // p.Close()なんて甘いことをやったら、ほとんどゴミが残る
                    // p.Close()の後にp.Killしても、関係が立たれてしまっているのでダメ。
                    p.Kill();
                }
                catch (Exception)
                {

                }

                p = null;
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }
}

