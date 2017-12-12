/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

 using Hidemaru;
using System;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Windows.Forms;

partial class HmTSHintPopupForm : Form
{
    private Label lb = new Label();
    private Label lbDetail = new Label();
    public static HmTSHintPopupForm form { get; set; }
    private String strFontName;
    public HmTSHintPopupForm(String fontname)
    {
        this.strFontName = fontname;
        SetFormAttr();
        SetLabelAttr();
        SetTimerAttr();
        SetTSServerAttr();
        StartTSServer();
    }

    private void SetTimerAttr()
    {
        timer = new Timer();
        timer.Tick += Timer_Tick;
        timer.Enabled = true;
        timer.Interval = 10;
        timer.Start();
    }

    private void SetFormAttr()
    {
        this.Width = 816;
        this.Height = 208;
        this.BackColor = Color.White;
        this.FormClosing += form_FormClosing;

        SetFormNoBorderAttr();
    }

    private void form_FormClosing(object sender, FormClosingEventArgs e)
    {
        Stop();
    }

    private void SetLabelAttr()
    {
        const int padding = 8;
        lb.Left = padding;
        lb.Top = padding;
        lb.Font = new Font(strFontName, 14);
        lb.Width = 800;
        lb.Height = 40;
        this.Controls.Add(lb);

        lbDetail.Left = padding;
        lbDetail.Top = lb.Bottom + padding;
        lbDetail.Font = new Font(strFontName, 14);
        lbDetail.Width = lb.Width;
        lbDetail.Height = 140;
        this.Controls.Add(lbDetail);
    }


    // 自分の位置をマウスの位置に従って移動
    private void MoveLocation()
    {
        this.Left = System.Windows.Forms.Cursor.Position.X;
        //Y座標を取得する
        this.Top = System.Windows.Forms.Cursor.Position.Y + 10;
    }

    // フォームの外淵に沿って四角を描画
    private void DrawRectangle()
    {
        Graphics g = this.CreateGraphics();
        Pen pen = new Pen(Color.Gray, 1);
        g.DrawRectangle(pen, 0, 0, this.Width - 1, this.Height - 1);
        pen = null;
        g = null;
    }

    private void HideForm()
    {
        hepResultInfo.Result = "";
        hepResultInfo.LineNo = -1;
        hepResultInfo.Column = -1;

        this.Hide();
    }

    private void ShowForm()
    {
        this.Show();
    }

    Timer timer;
    int nTickCounter = 0;
    private void Timer_Tick(object sender, EventArgs e)
    {
        MoveLocation();
        DrawRectangle();

        nTickCounter++;
        if (nTickCounter % (200/timer.Interval) != 0) // 手抜き
        {
            return;
        }

        // アクティブウィンドウがよそにある
        if (!IsActiveWindowIsHidemaruMainWindow())
        {
            HideForm();
            return;
        }


        if (!IsUnderWindowIsCurrentProcessWindow())
        {
            HideForm();
            return;
        }

        // 自分が先頭ではない
        IntPtr hWnd = Hm.WindowHandle;
        var list = GetWindowHidemaruHandleList();
        if (list.Count > 0 && list[0] != hWnd)
        {
            HideForm();
            return;
        }

        String strCurFileName = Hm.Edit.FilePath ?? "";
        bool isTSfile = TimerTick_Notify(strCurFileName);
        if (!isTSfile)
        {
            return;
        }

        try
        {
            SearchRegexpMatch(new Regex("^[a-zA-Z0-9_]+"));
            tsserver_Timer_Tick(null, null);
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.GetType());
            System.Diagnostics.Trace.WriteLine(ex.Message);
            System.Diagnostics.Trace.WriteLine(ex.StackTrace);
        }
    }

    class HmEditPosition
    {
        public int LineNo { get; set; }
        public int Column { get; set; }
        public String Result { get; set; }
        public String LbText { get; set; }

        public void Reset()
        {
            Result = "";
            LbText = "";
            LineNo = -1;
            Column = -1;
        }
    }

    private HmEditPosition hepResultInfo = new HmEditPosition();

    private String SearchRegexpMatch(Regex r)
    {
        // マウスの位置をLineNoとColumnへと投影
        var pos = Hm.Edit.MousePos;
        int lineno = pos.LineNo - 1;
        int column = pos.Column;

        // どちらかがはみ出ているならダメ
        if (lineno < 0 || column < 0)
        {
            HideForm();

            hepResultInfo.Reset();

            return "";
        }

        // テキスト全体を行単位に分解し
        String text = Hm.Edit.TotalText;
        char[] dlm = { '\n' };

        // １行を文字単位で扱えるようにする。
        String[] split = text.Split(dlm, StringSplitOptions.None);


        // マウスの下にある単語の抽出
        String result = "";

        // 今の位置から１つずつ前へと調査し、
        // マッチしたもの全体が、現在のマウスの下のcolumnの位置を含んでいれば良い。
        // その条件に見合うもののうち、最も長大マッチするものが求める単語となる。
        int result_ix = 0;
        for (int ix = column; ix >= 0; ix--)
        {
            String target = split[lineno].Substring(ix);
            Match m = r.Match(target);
            if (m.Success)
            {
                // マッチしたものが、現在のcolumnの位置を含むのか。
                String current = m.ToString();
                if (column < ix + current.Length)
                {
                    result_ix = ix;
                    result = m.ToString();
                }
            }
        }

        // 有効な単語が取得できたら
        if (result.Length > 0)
        {
            // このフォームを表示して
            ShowForm();

            hepResultInfo.Result = result;
            hepResultInfo.LineNo = pos.LineNo;
            hepResultInfo.Column = result_ix;
            hepResultInfo.LbText = result + "(lineno:" + pos.LineNo.ToString() + ", column:" + result_ix.ToString() + ")";

            // 現在ラベル内容を食い違うなら更新
            if ((String)lb.Text != hepResultInfo.LbText )
            {
                lb.Text = hepResultInfo.LbText;
                tsserver_ChangeTargetWord(pos.LineNo, hepResultInfo.Column + 1); // +1 するには、tsserverは、columnが１始まりだから。
            }

            return result;
        }
        // 有効でないなら、フォーム自体を隠す
        else
        {
            HideForm();

            hepResultInfo.Reset();

            // Labelの中身も消滅
            if (lb.Text != "")
            {
                lb.Text = "";
                lbDetail.Text = "";
            }

            return result;
        }
    }

    private void Stop()
    {
        try
        {
            if (timer != null)
            {
                timer.Stop();
                timer = null;
            }

            StopTSServer();

            isClosed = true;
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

}
