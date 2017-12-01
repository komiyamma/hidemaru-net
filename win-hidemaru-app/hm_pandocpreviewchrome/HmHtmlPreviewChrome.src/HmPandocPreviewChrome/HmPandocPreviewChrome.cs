using System;


public class HmPandocPreviewChrome
{
    public static IntPtr Show(IntPtr hWndHidemaru, IntPtr pmode)
    {
        HmPandocPreviewChromeForm.form = new HmPandocPreviewChromeForm(hWndHidemaru, pmode);

        return (IntPtr)1;
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            // まだ残っていたら(フォームを手動で閉じていたら、残っていない)
            if (HmPandocPreviewChromeForm.form != null)
            {
                //監視を終了
                HmPandocPreviewChromeForm.form.Stop();

                // 閉じてNull
                HmPandocPreviewChromeForm.form.Close();
                HmPandocPreviewChromeForm.form = null;

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

