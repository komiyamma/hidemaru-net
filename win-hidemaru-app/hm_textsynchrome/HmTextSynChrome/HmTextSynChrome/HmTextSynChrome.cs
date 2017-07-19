using System;


public class HmTextSynChrome
{
    public static IntPtr Show(IntPtr hWndHidemaru, String strOption1)
    {
        HmTextSynChromeForm.form = new HmTextSynChromeForm(hWndHidemaru, strOption1);

        return (IntPtr)1;
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            // まだ残っていたら(フォームを手動で閉じていたら、残っていない)
            if (HmTextSynChromeForm.form != null)
            {
                //監視を終了
                HmTextSynChromeForm.form.Stop();

                // 閉じてNull
                HmTextSynChromeForm.form.Close();
                HmTextSynChromeForm.form = null;

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

