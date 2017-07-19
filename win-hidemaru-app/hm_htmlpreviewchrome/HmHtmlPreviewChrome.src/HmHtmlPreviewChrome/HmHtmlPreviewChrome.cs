using System;


public class HmHtmlPreviewChrome
{
    public static IntPtr Show(IntPtr hWndHidemaru, IntPtr pmode)
    {
        HmHtmlPreviewChromeForm.form = new HmHtmlPreviewChromeForm(hWndHidemaru);

        return (IntPtr)1;
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            // まだ残っていたら(フォームを手動で閉じていたら、残っていない)
            if (HmHtmlPreviewChromeForm.form != null)
            {
                //監視を終了
                HmHtmlPreviewChromeForm.form.Stop();

                // 閉じてNull
                HmHtmlPreviewChromeForm.form.Close();
                HmHtmlPreviewChromeForm.form = null;

                // 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
                GC.Collect();
            }
        }
        catch (Exception)
        {
        }

        return (IntPtr)1;
    }
}

