/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

 using System;
using System.Windows.Forms;

// これらはアクティブ化しないための特殊な施策。
partial class HmTSHintPopupForm : Form
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

