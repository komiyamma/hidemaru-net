using System;
using System.Diagnostics;

public class HmPrompt
{
    public static IntPtr Show(IntPtr command)
    {
        try
        {
            // う～ん、どうもとにかくクローズしてしまった方がよさそう。
            Close();
            HmPromptForm.form = new HmPromptForm(command);

            /*
            // ヌル
            if (HmPromptForm.form == null) {
                HmPromptForm.form = new HmPromptForm(command);
            // インスタンスは生きてるが、プロセスが閉じてる
            } else if (HmPromptForm.form.IsClose()) {
                Close();
                HmPromptForm.form = new HmPromptForm(command);
            // コマンドのタイプが違う
            } else if (command != HmPromptForm.form.GetCommandType())
            {
                Close();
                HmPromptForm.form = new HmPromptForm(command);
            }
            */
        }
        catch (Exception)
        {

        }

        return (IntPtr)1;
    }

    public static IntPtr Close()
    {
        try
        {
            // まだ残っていたら(フォームを手動で閉じていたら、残っていない)
            if (HmPromptForm.form != null)
            {
                // 閉じてNull
                HmPromptForm.form.Close();
                HmPromptForm.form = null;

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

    public static IntPtr OnDetachMethod()
    {
        return Close();
    }
}

