using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Hidemaru;

namespace HmEnglishTextWordCounter
{

    internal partial class WordCounter
    {
        string cmdFormat = "";
        string splitBy = " ";
        string preLastString = "";

        Task task;
        CancellationTokenSource tokenSource;
        public WordCounter(string cmdFormat, string splitBy)
        {
            this.cmdFormat = cmdFormat;
            this.splitBy = splitBy;
            CreateTimer();
        }

        ~WordCounter()
        {
            StopTimer();
        }

        void HeavyMethod(CancellationToken token)
        {
            while (true)
            {
                if (token.IsCancellationRequested) { break; }

                bool is_must_update = false;

                string targetText = "";

                string selectedText = Hm.Edit.SelectedText ?? "";
                if (selectedText != "")
                {
                    targetText = selectedText;
                    if (selectedText != preLastString)
                    {
                        is_must_update = true;
                        preLastString = selectedText;
                    }
                }
                else
                {
                    string totalText = Hm.Edit.TotalText ?? "";
                    targetText = totalText;
                    if (preLastString != totalText)
                    {
                        is_must_update = true;
                        preLastString = totalText;
                    }
                }

                // マクロは実行中ではない
                if (is_must_update && !Hm.Macro.IsExecuting)
                {
                    char[] splitWordsBy = "\n\t .,:;\"'?!".ToArray();
                    List<String> wordsArray = new List<String>(targetText.Split(splitWordsBy));
                    wordsArray = wordsArray.FindAll((s) => s.Length > 0);
                    var command = cmdFormat + ";\n\nendmacro \"EndMacro\";\n";
                    command = command.Replace("{WORD_COUNT}", wordsArray.Count.ToString());
                    var ret = Hm.Macro.Exec.Eval(command);
                    // 最後まで実行できた？
                    if (ret.Message == "EndMacro")
                    {
                        ;
                    }
                    else
                    {
                        System.Diagnostics.Trace.WriteLine("マクロ中にエラーが発生している可能性があります。");
                        this.StopTimer();
                    }
                }

                Thread.Sleep(1000);
            }
        }

        public void CreateTimer()
        {
            // timer.Start();

            tokenSource = new CancellationTokenSource();
            var token = tokenSource.Token;

            task = Task.Run(() => HeavyMethod(token));

        }

        public void StopTimer()
        {
            if (tokenSource != null)
            {
                tokenSource.Cancel();
            }
        }
    }

    public class HmEnglishTextWordCounnter
    {
        static WordCounter form;
        public static IntPtr StartWordCounter(String cmdFormat, String splitBy)
        {
            StopWordCounter();
            if (form == null)
            {
                form = new WordCounter(cmdFormat, splitBy);
            }

            return (IntPtr)1;
        }

        public static IntPtr IsCountMode()
        {
            if (form != null)
            {
                return (IntPtr)1;
            }
            else
            {
                return (IntPtr)0;
            }
        }

        public static IntPtr StopWordCounter()
        {
            if (form != null)
            {
                form.StopTimer();
                form = null;
            }

            return (IntPtr)0;
        }


        public static IntPtr DetachWordCounter(IntPtr status)
        {
            if (form != null)
            {
                form.StopTimer();
                form = null;
            }

            return (IntPtr)1;
        }
    }
}

