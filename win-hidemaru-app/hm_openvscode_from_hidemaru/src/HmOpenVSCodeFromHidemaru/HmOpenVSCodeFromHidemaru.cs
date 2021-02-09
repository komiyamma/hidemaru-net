using Hidemaru;
using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace HmOpenVSCodeFromHidemaru
{
    public class HmOpenVSCodeFromHidemaru
    {
        const string hm_macro_error_msg_variable = "$ERROR_MSG";

        const string strCmdGit = "git";
        const string strCmdGit_RevParse_GitDir = "rev-parse --git-dir";
        // const string strCmdGit_Args_RevParse_IsInSideWorkTree = "rev-parse --is-inside-work-tree";

        static Process pCmdGit = new Process();

        const string strGitErrorMessage = "Gitコマンドに失敗しています。";

        string targetFileDirectory = "";

        string strCurrentDirectory = "";

        HmOpenVSCodeFromHidemaru(string targetFilePath)
        {
            SaveTargetFileDirectory(targetFilePath);
        }

        ~HmOpenVSCodeFromHidemaru()
        {
        }

        public void SaveTargetFileDirectory(string targetFilePath)
        {
            if (File.Exists(targetFilePath))
            {
                var dir = Path.GetDirectoryName(targetFilePath);
                this.targetFileDirectory = dir;
            }
            else
            {
                throw new FileNotFoundException(targetFileDirectory);
            }
        }

        public string GetAttributeGitDirectory()
        {
            strCurrentDirectory = Directory.GetCurrentDirectory();

            Directory.SetCurrentDirectory(this.targetFileDirectory); // コマンド実行のためやむなし

            ProcessStartInfo pCmdGitPSInfo = new ProcessStartInfo();
            pCmdGitPSInfo.FileName = strCmdGit;
            pCmdGitPSInfo.Arguments = strCmdGit_RevParse_GitDir;
            pCmdGitPSInfo.CreateNoWindow = true; // コンソール・ウィンドウを開かない
            pCmdGitPSInfo.UseShellExecute = false; // シェル機能を使用しない
            pCmdGitPSInfo.RedirectStandardOutput = true; // 標準出力をリダイレクト

            pCmdGit.StartInfo = pCmdGitPSInfo;

            try
            {
                bool isCommandSuccess = pCmdGit.Start();
                if (isCommandSuccess)
                {
                    string strCommandResult = pCmdGit.StandardOutput.ReadToEnd();
                    strCommandResult = strCommandResult.Trim();

                    // github リポジトリ下であれば"ture"という文字が返ってくる。
                    if (strCommandResult == ".git")
                    {
                        return "";
                    }

                    try
                    {
                        strCommandResult = Regex.Replace(strCommandResult, "/.git$", "");
                        string strGitDirectory = Path.GetFullPath(strCommandResult);
                        if (Directory.Exists(strGitDirectory))
                        {
                            return strGitDirectory;
                        }
                        else
                        {
                            new DirectoryNotFoundException(strGitDirectory);
                        }
                    }
                    catch (Exception)
                    {
                        return "";
                    }

                } else
                {
                    throw new InvalidOperationException(strCmdGit);
                }
            }
            catch (Exception e)
            {
                Hm.Macro.Var[hm_macro_error_msg_variable] = strGitErrorMessage;
            }
            finally
            {
                Directory.SetCurrentDirectory(strCurrentDirectory);
            }

            return "";
        }

        public static IntPtr try_parse_git_dir()
        {
            string target_file = Hm.Edit.FilePath ?? "";
            if (String.IsNullOrEmpty(target_file))
            {
                Hm.Macro.Var[hm_macro_error_msg_variable] = "(無題)であるためVisual Studio Codeで開くことが出来ません。";
                return (IntPtr)0;
            }
            if (!File.Exists(target_file))
            {
                Hm.Macro.Var[hm_macro_error_msg_variable] = "対象のファイルは存在しません。";
                return (IntPtr)0;
            }

            try
            {
                HmOpenVSCodeFromHidemaru cmd = new HmOpenVSCodeFromHidemaru(target_file);
                string git_dir = cmd.GetAttributeGitDirectory();

                Hm.Macro.Var["#LINENO_UNICODE"] = Hm.Edit.CursorPos.LineNo;
                Hm.Macro.Var["#COLUMN_UNICODE"] = Hm.Edit.CursorPos.Column;

                Hm.Macro.Var["$GIT_DIRECTORY"] = git_dir;
                return (IntPtr)1;

            }
            catch (Exception e)
            {
                var args_path_dir = Path.GetDirectoryName(target_file);
                Hm.Macro.Var[hm_macro_error_msg_variable] = strGitErrorMessage;
                return (IntPtr)0;
            }
        }
    }
}
