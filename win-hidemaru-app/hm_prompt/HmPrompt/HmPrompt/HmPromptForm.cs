/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;
using System.Windows.Forms;

internal partial class HmPromptForm
{
    static public HmPromptForm form { get; set; }

    Timer timer = new Timer();

    public HmPromptForm(ProcessStartInfo psi, Boolean processSettingMode)
    {
        this.constructPsi = psi.Copy();
        this.processSettingMode = processSettingMode;
        isClose = false;
        InitProcessAttr();

        SetTimerAttr();

    }

    private void SetTimerAttr()
    {
        timer.Interval = 30;
        timer.Enabled = true;
        timer.Start();
        timer.Tick += timer_Tick;
    }

    private void ClearTimer()
    {
        timer.Enabled = false;
        timer.Stop();
        timer = null;
    }

    private void timer_Tick(object sender, EventArgs e)
    {
        this.Update_TickProcessWindow(sender, e);
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
            ClearTimer();

            isClose = true;

            if (process != null)
            {
                try
                {
                    // p.Close()なんて甘いことをやったら、ほとんどゴミが残る
                    // p.Close()の後にp.Killしても、関係が立たれてしまっているのでダメ。
                    process.Kill();
                }
                catch (Exception)
                {

                }

                process = null;
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }
}

