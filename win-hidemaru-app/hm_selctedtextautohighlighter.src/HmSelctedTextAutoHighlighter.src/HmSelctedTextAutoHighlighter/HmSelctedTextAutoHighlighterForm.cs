using Hidemaru;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

public class HmSelctedTextAutoHighlighterForm : Form
{
    public static HmSelctedTextAutoHighlighterForm form;
    private Timer timer;
    private int KeepMonitoringMilliSeconds = 1000;
    private List<string> buffer;
    private string strLastText = "";

    public HmSelctedTextAutoHighlighterForm()
    {
        buffer = new List<string>();

        timer = new Timer();
        timer.Interval = 300;
        timer.Tick += timer_Tick;
        timer.Enabled = true;
        timer.Start();
    }

    private bool TryUpdateHidemaruMarker(string text)
    {
        if (!Hm.Macro.IsExecuting)
        {
            if (text.Length == 0)
            {

                var ret = Hm.Macro.Exec.Eval(
                @"
                    deletecolormarker ""HmSelctedTextTmpColorMarker"", 33001;
                    endmacro ""CompleteClear"";
                ");

                if (ret.Message == "CompleteClear")
                {
                    return true;
                }

            }
            else { 

                var ret = Hm.Macro.Exec.Eval(
                @"
                    setcompatiblemode 0x20000;
                    deletecolormarker ""HmSelctedTextTmpColorMarker"", 33001;
                    $text = gettext(seltopx, seltopy, selendx, selendy, 1);
                    setsearch $text, 0x00000802;
                    hilightfound 1;
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



    private void timer_Tick(object sender, EventArgs e)
    {
        try
        {
            Timer timer = sender as Timer;

            var text = Hm.Edit.SelectedText;

            // 長すぎる文字列をハイライトとか変。やめ
            if (text.Length > 100)
            {
                return;
            }

            buffer.Add(Hm.Edit.SelectedText);

            if (buffer.Count > KeepMonitoringMilliSeconds / timer.Interval)
            {
                buffer.RemoveAt(0);

                if (text != strLastText)
                {
                    if (buffer.TrueForAll(tx => tx == text))
                    {
                        if (TryUpdateHidemaruMarker(text))
                        {
                            strLastText = text;
                        }
                    }
                }
            }
        }
        catch (Exception)
        {
        }
    }

    public void Stop()
    {
        timer.Enabled = false;
        timer.Stop();
        timer = null;
    }
}
