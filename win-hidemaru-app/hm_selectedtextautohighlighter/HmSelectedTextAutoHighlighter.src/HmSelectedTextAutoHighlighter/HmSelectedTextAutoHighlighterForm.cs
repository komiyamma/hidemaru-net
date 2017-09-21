using Hidemaru;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

public class HmSelectedTextAutoHighlighterForm : Form
{
    public static HmSelectedTextAutoHighlighterForm form;
    private Timer timer;
    private const int KeepMonitoringMilliSeconds = 1000; // 監視する上で、文字列選択が「固定した」とする目安時間。これプラス
    private const int ItvlMonitoringMilliSeconds = 300; // 基本的な監視の「チェック」間隔
    private const int mustMonitoringCount = KeepMonitoringMilliSeconds / ItvlMonitoringMilliSeconds;  // モニターすべきカウント数

    private List<string> bufferSelectedText = new List<string>();
    private string strLastHighlightSelectedText = "";

    public HmSelectedTextAutoHighlighterForm()
    {
        SetTimerAttribute();
    }

    private void SetTimerAttribute()
    {
        timer = new Timer()
        {
            Interval = ItvlMonitoringMilliSeconds,
            Enabled = true,
        };

        timer.Tick += timer_Tick;

        timer.Start();
    }

    // 秀丸のマーカー状態を更新する
    private bool TryUpdateHidemaruMarker(string text)
    {
        if (!Hm.Macro.IsExecuting)
        {
            // テキストを選択していない、もしくは、テキストが長すぎる
            if (text.Length == 0 || text.Length > 100)
            {
                var ret = Hm.Macro.Exec.Eval(
                @"
                    deletecolormarker ""HmSelectedTextTmpColorMarker"", 33001;
                    endmacro ""CompleteClear"";
                ");

                if (ret.Message == "CompleteClear")
                {
                    return true;
                }

            }
            else
            {

                var ret = Hm.Macro.Exec.Eval(
                @"
                    setcompatiblemode 0x20000;
                    deletecolormarker ""HmSelectedTextTmpColorMarker"", 33001;
                    $text = gettext(seltopx, seltopy, selendx, selendy, 1);
                    setsearch $text, 0x00000002;
                    colormarkerallfound 0x333333,0x8CE6F0,-1,1,33001,""HmSelectedTextTmpColorMarker"";
                    endmacro ""CompleteHighlight"";
                ");

                if (ret.Message == "CompleteHighlight")
                {
                    return true;
                }
            }
        }
        return false;
    }

    // 選択しているテキストが「安定した」という条件
    private bool IsConditionMonitoringPass(String text)
    {

        if (bufferSelectedText.Count >= mustMonitoringCount)
        {
            if (bufferSelectedText.TrueForAll(t => t == text))
            {
                return true;
            }
        }

        return false;
    }

    // 選択しているテキストをTickごとに蓄積するためのQueueにためこむ
    private void AddQueueBufferSelectedText(String text)
    {
        // バッファーに加える
        bufferSelectedText.Add(text);

        // モニターすべきカウント数より、バッファーのカウントが多かったら、先頭から取り除く
        while (true)
        {
            if (bufferSelectedText.Count > mustMonitoringCount)
            {
                bufferSelectedText.RemoveAt(0);
            }
            else
            {
                break;
            }
        }
    }

    private void timer_Tick(object sender, EventArgs e)
    {
        try
        {
            var text = Hm.Edit.SelectedText;

            AddQueueBufferSelectedText(text);

            if (text != strLastHighlightSelectedText)
            {
                if (IsConditionMonitoringPass(text))
                {
                    if (TryUpdateHidemaruMarker(text))
                    {
                        strLastHighlightSelectedText = text;
                    }
                }
            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex);
        }
    }

    public void Stop()
    {
        timer.Enabled = false;
        timer.Stop();
        timer = null;
    }
}
