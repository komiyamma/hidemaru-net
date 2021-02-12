using Hidemaru;
using Microsoft.Win32;
using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace HmOpenVSCodeFromHidemaru
{
    public class HmOpenVSCodeFromHidemaru
    {
        const string strHmMacroErrorMsgVariable = "$ERROR_MSG";

        const string strHmMacroVSCodeAbsolutePathVariable = "$VSCODE_ABSOLUTE_PATH";

        const string strCmdGit = "git";
        const string strCmdGit_RevParse_GitDir = "rev-parse --git-dir";
        // const string strCmdGit_Args_RevParse_IsInSideWorkTree = "rev-parse --is-inside-work-tree";

        Process pCmdGit = new Process();

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

        private string GetVisualStudioCodePath()
        {
            string vscode_path = (string)Hm.Macro.Var[strHmMacroVSCodeAbsolutePathVariable];
            if (String.IsNullOrEmpty(vscode_path))
            {
                // 操作するレジストリ・キーの名前
                string rKeyName = @"Applications\Code.exe\shell\open";
                // 取得処理を行う対象となるレジストリの値の名前
                string rGetValueName = "Icon";

                // レジストリの取得
                try
                {
                    // レジストリ・キーのパスを指定してレジストリを開く
                    RegistryKey rKey = Registry.ClassesRoot.OpenSubKey(rKeyName);
                    // レジストリの値を取得

                    vscode_path = (string)rKey.GetValue(rGetValueName);

                    // 開いたレジストリ・キーを閉じる
                    rKey.Close();
                }
                catch (NullReferenceException e)
                {
                    vscode_path = "";
                }
            }

            if (String.IsNullOrEmpty(vscode_path))
            {
                // 操作するレジストリ・キーの名前
                string rKeyName = @"SOFTWARE\Classes\Applications\Code.exe\shell\open";
                // 取得処理を行う対象となるレジストリの値の名前
                string rGetValueName = "Icon";

                // レジストリの取得
                try
                {
                    // レジストリ・キーのパスを指定してレジストリを開く
                    RegistryKey rKey = Registry.CurrentUser.OpenSubKey(rKeyName);
                    // レジストリの値を取得

                    vscode_path = (string)rKey.GetValue(rGetValueName);

                    // 開いたレジストリ・キーを閉じる
                    rKey.Close();
                }
                catch (NullReferenceException e)
                {
                    vscode_path = "";
                }
            }

            return vscode_path;
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
                        return this.targetFileDirectory;
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
                Hm.Macro.Var[strHmMacroErrorMsgVariable] = strGitErrorMessage;
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
                Hm.Macro.Var[strHmMacroErrorMsgVariable] = "(無題)であるためVisual Studio Codeで開くことが出来ません。";
                return (IntPtr)0;
            }
            if (!File.Exists(target_file))
            {
                Hm.Macro.Var[strHmMacroErrorMsgVariable] = "対象のファイルは存在しません。";
                return (IntPtr)0;
            }

            try
            {
                Hm.Macro.Var["#LINENO_UNICODE"] = Hm.Edit.CursorPos.LineNo;
                Hm.Macro.Var["#COLUMN_UNICODE"] = Hm.Edit.CursorPos.Column;

                HmOpenVSCodeFromHidemaru cmd = new HmOpenVSCodeFromHidemaru(target_file);
                string git_dir = cmd.GetAttributeGitDirectory();
                Hm.Macro.Var["$GIT_DIRECTORY"] = git_dir;

                string vscode_path = cmd.GetVisualStudioCodePath();
                vscode_path = vscode_path.Replace("\"", ""); // ダブルコーテーションは削除
                Hm.Macro.Var[strHmMacroVSCodeAbsolutePathVariable] = vscode_path;

                cmd = null;

                return (IntPtr)1;

            }
            catch (Exception e)
            {
                var args_path_dir = Path.GetDirectoryName(target_file);
                Hm.Macro.Var[strHmMacroErrorMsgVariable] = strGitErrorMessage;
                return (IntPtr)0;
            }
        }
    }
}
