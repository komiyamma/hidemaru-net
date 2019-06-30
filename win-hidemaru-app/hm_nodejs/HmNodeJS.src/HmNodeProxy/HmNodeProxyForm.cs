/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Windows.Forms;

class HmNodeProxyForm : Form
{
    static System.Windows.Forms.Timer myTimer = new System.Windows.Forms.Timer();
    public static RichTextBox rtb = new RichTextBox();
    int IP = 0;
    bool bShowWindow;
    public HmNodeProxyForm(int IP, bool bShowWindow)
    {
        this.IP = IP;
        this.bShowWindow = bShowWindow;
        SetTimerAttribute();
        SetFormAttribute();
        SetIconAttribute();
    }

    // フォームの設定
    void SetFormAttribute()
    {
        // 見えない、枠がない、
        this.Visible = false;

        this.StartPosition = FormStartPosition.Manual;

        this.FormBorderStyle = FormBorderStyle.None;
        this.Text = IP.ToString();
        this.ControlBox = false;
        this.WindowState = FormWindowState.Minimized;
        if (bShowWindow)
        {
            this.ShowInTaskbar = true;

        }
        else
        {
            this.ShowInTaskbar = false; // これをやってしまうと、p.MainWindowHandleでは見えない、ProcessExtensionsのようにネイティブをラップしてアクセスする。
        }
        this.Width = 0;
        this.Height = 1;
        this.Left = 0;
        this.Top = 0;

        this.FormClosing += new FormClosingEventHandler(form_FormClosing);

    }

    // フォームアイコンの設定
    void SetIconAttribute()
    {
        System.Reflection.Assembly asm = System.Reflection.Assembly.GetExecutingAssembly();
        System.Resources.ResourceManager rm = new System.Resources.ResourceManager(asm.GetName().Name + ".resource", asm);

        this.Icon = (System.Drawing.Icon)(rm.GetObject("icon_node"));
    }

    // フォームタイマーの設定
    void SetTimerAttribute()
    {
        myTimer.Tick += new EventHandler(TimerEventProcessor);

        // Sets the timer interval to 5 seconds.
        myTimer.Interval = 300;
        myTimer.Start();
    }

    void TimerEventProcessor(Object o, EventArgs e)
    {
        // 秀丸が無いなら、終わる
        if (HidemaruWindowInfo.IsFindWindow())
        {
            ;
        }
        else
        {
            string mapfilename = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\HmNodeProxy.txt";
            try
            {
                System.IO.File.Delete(mapfilename);
            }
            catch (Exception ex) { System.Diagnostics.Trace.WriteLine(ex.Message); }

            myTimer.Stop();
            this.Close();
        }
    }

    void form_FormClosing(Object o, FormClosingEventArgs e)
    {
        // System.Diagnostics.Trace.WriteLine("閉じる");
    }

}
