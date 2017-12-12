/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;

public class HmPrompt
{
    static ProcessStartInfo psi = new ProcessStartInfo();

    public static IntPtr ProcessStartInfo(String processFileName, String processArguments, String processWorkingDirectory)
    {
        HmPrompt.psi = new ProcessStartInfo {
            FileName = processFileName,
            Arguments = processArguments,
            WorkingDirectory = processWorkingDirectory
        };

        return (IntPtr)1;
    }

    public static IntPtr Show(IntPtr processSettingMode)
    {
        try
        {
            // Close()すると消えるので先に確保しておく
            ProcessStartInfo cpyPsi = psi.Copy();

            // う～ん、どうもとにかくクローズしてしまった方がよさそう。
            Close();

            HmPromptForm.form = new HmPromptForm(cpyPsi, (processSettingMode.ToInt32() != 0));
            psi.Clear();
        }
        catch (Exception)
        {

        }

        return (IntPtr)1;
    }

    public static IntPtr Close()
    {
        psi.Clear();
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

// 拡張。コピーとクリア
internal static class ProcessStartInfoExtension
{
    public static void Clear(this ProcessStartInfo psi)
    {
        psi.FileName = "";
        psi.Arguments = "";
        psi.WorkingDirectory = "";
    }
    public static ProcessStartInfo Copy(this ProcessStartInfo srcPsi)
    {
        ProcessStartInfo dstPsi= new ProcessStartInfo
        {
            FileName = srcPsi.FileName,
            Arguments = srcPsi.Arguments,
            WorkingDirectory = srcPsi.WorkingDirectory
        };

        return dstPsi;
    }
}