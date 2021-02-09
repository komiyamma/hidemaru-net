using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Hidemaru;

namespace HmOpenVSCodeFromHidemaru
{
    public class HmOpenVSCodeFromHidemaru
    {
        const string hm_macro_error_msg_variable = "$ERROR_MSG";

        const string cmd_git = "git";
        const string cmd_git_args_revparse_gitdir = "rev-parse --git-dir";
        // const string cmd_git_args_revparse_is_inside_worktree = "rev-parse --is-inside-work-tree";

        static Process pCmdGit = new Process();

        string targetDirectory = "";
        HmOpenVSCodeFromHidemaru(string targetDirectory)
        {
            SetTargetDirectory(targetDirectory);
        }

        private void SetTargetDirectory(string targetFile)
        {
            if (File.Exists(targetFile))
            {
                var dir = Path.GetDirectoryName(targetFile);
                this.targetDirectory = dir;
                Directory.SetCurrentDirectory(dir);
            }
            else
            {
                throw new FileNotFoundException(targetDirectory);
            }
        }

        public string GitProcessExecute()
        {
            ProcessStartInfo pCmdGitPSInfo = new ProcessStartInfo();
            pCmdGitPSInfo.FileName = cmd_git;
            pCmdGitPSInfo.Arguments = cmd_git_args_revparse_gitdir;
            pCmdGitPSInfo.CreateNoWindow = true; // コンソール・ウィンドウを開かない
            pCmdGitPSInfo.UseShellExecute = false; // シェル機能を使用しない
            pCmdGitPSInfo.RedirectStandardOutput = true; // 標準出力をリダイレクト

            pCmdGit.StartInfo = pCmdGitPSInfo;

            try
            {
                bool ret_1st_command = pCmdGit.Start();
                if (ret_1st_command)
                {
                    string result = pCmdGit.StandardOutput.ReadToEnd();
                    result = result.Trim();

                    // github リポジトリ下であれば"ture"という文字が返ってくる。
                    if (result == ".git")
                    {
                        return targetDirectory;
                    }

                    try
                    {
                        result = result.Remove(result.Length - 4);
                        if (Directory.Exists(result))
                        {
                            return Path.GetFullPath(result);
                        }
                        else
                        {
                            new DirectoryNotFoundException();
                        }
                    }
                    catch (Exception)
                    {
                        return targetDirectory;
                    }

                } else
                {
                    Hm.Macro.Var[hm_macro_error_msg_variable] = "Gitコマンドに失敗しています。";
                }
            }
            catch (Exception e)
            {
                Hm.Macro.Var[hm_macro_error_msg_variable] = "Gitコマンドに失敗しています。";
            }

            return targetDirectory;
        }

        public static IntPtr try_parse_git_dir()
        {
            string target_file = Hm.Edit.FilePath ?? "";
            if (String.IsNullOrEmpty(target_file))
            {
                Hm.Macro.Var[hm_macro_error_msg_variable] = "(無題)であるためVisual Studio Codeで開くことが出来ません。";
            }
            try
            {
                HmOpenVSCodeFromHidemaru cmd = new HmOpenVSCodeFromHidemaru(target_file);
                string result = cmd.GitProcessExecute();

                Hm.Macro.Var["#LINENO_UNICOD"] = Hm.Edit.CursorPos.LineNo;
                Hm.Macro.Var["#COLUMN_UNICOD"] = Hm.Edit.CursorPos.Column;

                Hm.Macro.Var["$TARGET_DIRECTORY"] = Path.GetDirectoryName(result);
                return (IntPtr)1;

            }
            catch (Exception e)
            {
                var args_path_dir = Path.GetDirectoryName(target_file);
                Hm.Macro.Var[hm_macro_error_msg_variable] = "Gitコマンドに失敗しています。";
                return (IntPtr)0;
            }
        }
    }
}
