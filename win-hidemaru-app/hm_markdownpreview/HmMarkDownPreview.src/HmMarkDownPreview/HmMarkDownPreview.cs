using System;

public class HmMarkDownPreview
{
    public static IntPtr Show(IntPtr hWndHidemaru) { 
        // インスタンスをフォーム自身に保持させて、フォームを表示。
        HmMarkDownPreviewForm.form = new HmMarkDownPreviewForm(hWndHidemaru);
        HmMarkDownPreviewForm.form.Show();

        return (IntPtr)1;
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            if (HmMarkDownPreviewForm.form != null)
            {
                // 音を戻す
                HmMarkDownPreviewForm.SetOn_IESoundEffects();

                // 先にタイマーをストップ
                HmMarkDownPreviewForm.form.update.Stop();
                HmMarkDownPreviewForm.form.update.Enabled = false;

                // 閉じてNull
                HmMarkDownPreviewForm.form.Close();
                HmMarkDownPreviewForm.form = null;

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
