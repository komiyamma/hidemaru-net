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
		timer.Enabled = true;
		timer.Start();
		timer.Tick += timer_Tick;
	}

	private void UpdateHidemaruMarker(string text)
	{
		if (!Hm.Macro.IsExecuting)
		{
			var ret = Hm.Macro.Exec.Eval(
            @"
                setcompatiblemode 0x20000;
                deletecolormarker ""HmSelctedTextTmpColorMarker"", 33001;
                $text = gettext(seltopx, seltopy, selendx, selendy, 1);
                setsearch $text, 0x00000002;
                colormarkerallfound 0x333333,0x8CE6F0,-1,1,33001,""HmSelctedTextTmpColorMarker"";
                endmacro ""Complete"";
            ");

            if (ret.Message == "Complete")
            {
                strLastText = text;
            }
        }
	}



	private void timer_Tick(object sender, EventArgs e)
	{
		try
		{
			Timer timer = sender as Timer;

            var text = Hm.Edit.SelectedText;
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
                        UpdateHidemaruMarker(text);
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
