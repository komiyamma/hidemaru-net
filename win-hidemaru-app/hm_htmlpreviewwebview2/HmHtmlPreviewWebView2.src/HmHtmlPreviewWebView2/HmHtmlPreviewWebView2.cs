/* 
 * Copyright (c) 2021-2021 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;


public class HmHtmlPreviewWebView2
{
    public static IntPtr Show(IntPtr hWndHidemaru, IntPtr pMode)
    {
        try
        {
            int kMode = pMode.ToInt32();

            // インスタンスをフォーム自身に保持させて…
            if ((HmHtmlPreviewMode)kMode == HmHtmlPreviewMode.Live)
            {
                HmHtmlBaseForm.form = new HmHtmlLiveForm(hWndHidemaru);
            }
            else if ((HmHtmlPreviewMode)kMode == HmHtmlPreviewMode.File)
            {
                HmHtmlBaseForm.form = new HmHtmlFileForm(hWndHidemaru);
            }

            // 想定外の数値
            else
            {
                HmHtmlBaseForm.form = new HmHtmlLiveForm(hWndHidemaru);
            }

            // フォームを表示。
            HmHtmlBaseForm.form.Show();
        } catch(Exception)
        {

        }

        return (IntPtr)1;
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            // まだ残っていたら(フォームを手動で閉じていたら、残っていない)
            if (HmHtmlBaseForm.form != null)
            {

                // Stopの実行
                HmHtmlBaseForm.form.Stop();

                // 閉じてNull
                HmHtmlBaseForm.form.Close();
                HmHtmlBaseForm.form = null;

                // 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
        }
        catch (Exception)
        {
        }

        return (IntPtr)1;
    }
}

