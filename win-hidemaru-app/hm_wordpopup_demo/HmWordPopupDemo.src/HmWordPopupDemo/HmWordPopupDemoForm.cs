using Hidemaru;
using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;

partial class HmWordPopupDemoForm : Form
{
    Label lb = new Label();
    public static Form form { get; set; }
    public static String fontname;
    public HmWordPopupDemoForm()
    {
        SetFormAttr();
        SetLabelAttr();
        SetTimerAttr();
    }

    private void SetTimerAttr()
    {
        timer = new Timer();
        timer.Tick += Timer_Tick;
        timer.Enabled = true;
        timer.Interval = 15;
        timer.Start();
    }

    private void SetFormAttr()
    {
        this.Width = 516;
        this.Height = 258;
        this.BackColor = Color.White;
        this.FormClosing += ProgramForm_FormClosing;

        SetFormNoBorderAttr();
    }

    private void ProgramForm_FormClosing(object sender, FormClosingEventArgs e)
    {
        Stop();
    }

    private void SetLabelAttr()
    {
        lb.Left = 8;
        lb.Top = 8;
        lb.Font = new Font(fontname, 14);
        lb.Width = 500;
        this.Controls.Add(lb);
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
        resultInfo.result = "";
        resultInfo.LineNo = -1;
        resultInfo.Column = -1;

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
        if (nTickCounter % 15 != 0)
        {
            return;
        }
        try
        {
            SearchRegexpMatch(new Regex("^[a-zA-Z0-9_.]+"));
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.GetType());
            System.Diagnostics.Trace.WriteLine(ex.Message);
            System.Diagnostics.Trace.WriteLine(ex.StackTrace);
        }
    }

    class Position
    {
        public int LineNo { get; set; }
        public int Column { get; set; }
        public String result { get; set; }
    }

    private Position resultInfo = new Position();

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
            return "";
        }

        // アクティブウィンドウがよそにある
        if (!IsActiveWindowIsHidemaruMainWindow())
        {
            HideForm();
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
            // 現在ラベル内容を食い違うなら更新
            if (result != lb.Text)
            {
                lb.Text = result + "(lineno:" + pos.LineNo.ToString() + ", column:" + result_ix.ToString() + ")";
            }

            resultInfo.result = result;
            resultInfo.LineNo = pos.LineNo;
            resultInfo.Column = result_ix;

            return result;
        }
        // 有効でないなら、フォーム自体を隠す
        else
        {
            HideForm();
            // Labelの中身も消滅
            if (lb.Text != "")
            {
                lb.Text = "";
            }

            return result;
        }
    }

    [DllImport("user32.dll")]
    static extern IntPtr GetActiveWindow();

    [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern int GetClassName(IntPtr hWnd, StringBuilder lpClassName, int nMaxCount);

    private bool IsActiveWindowIsHidemaruMainWindow()
    {
        IntPtr hWnd = GetActiveWindow();
        StringBuilder ClassName = new StringBuilder(256);
        int nRet = GetClassName(hWnd, ClassName, ClassName.Capacity);
        if (ClassName.ToString().Contains("Hidemaru32Class"))
        {
            return true;
        }
        return false;
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
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

}

// これらはアクティブ化しないための特殊な施策。
partial class HmWordPopupDemoForm : Form
{
    private void SetFormNoBorderAttr()
    {
        //タイトルバーを消す
        this.ControlBox = false;
        this.Text = "";
        // ボーダーを消す
        this.FormBorderStyle = FormBorderStyle.None;
    }


    // フォーム表示時にアクティブにならないようにする
    protected override bool ShowWithoutActivation
    {
        get
        {
            return true;
        }
    }

    // このフォームがクリックなどされた時にアクティブにならないようにする。
    const int WM_MOUSEACTIVATE = 0x0021;
    const int MA_NOACTIVATE = 3;
    protected override void WndProc(ref Message m)
    {

        if (m.Msg == WM_MOUSEACTIVATE)
        {
            m.Result = new IntPtr(MA_NOACTIVATE);
            return;
        }

        base.WndProc(ref m);
    }


    // 常に最前面に表示させる(topMostプロパティを使うと
    // ShowWithoutActivationが効かないため
    private const int WS_EX_TOPMOST = 0x00000008;
    protected override CreateParams CreateParams
    {
        get
        {
            CreateParams p = base.CreateParams;
            p.ExStyle |= WS_EX_TOPMOST;
            return p;
        }
    }

}

