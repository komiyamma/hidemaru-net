using System;
using System.Diagnostics;

public class HmPrompt
{
    static String command = "";
    static String arguments = "";
    public static IntPtr ProcessStartInfo(String command, String arguments)
    {
        HmPrompt.command = command;
        HmPrompt.arguments = arguments;
        return (IntPtr)1;
    }

    public static IntPtr Show(IntPtr hasAttr)
    {
        try
        {
            // Close()すると消えるので先に確保しておく
            String command = HmPrompt.command;
            String arguments = HmPrompt.arguments;

            // う～ん、どうもとにかくクローズしてしまった方がよさそう。
            Close();

            HmPromptForm.form = new HmPromptForm(command, arguments, (hasAttr.ToInt32() != 0));
            HmPrompt.command = "";
            HmPrompt.arguments = "";
        }
        catch (Exception)
        {

        }

        return (IntPtr)1;
    }

    public static IntPtr Close()
    {
        HmPrompt.command = "";
        HmPrompt.arguments = "";
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

