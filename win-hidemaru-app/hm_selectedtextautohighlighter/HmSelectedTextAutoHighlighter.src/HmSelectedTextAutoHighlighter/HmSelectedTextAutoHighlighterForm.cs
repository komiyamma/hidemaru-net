/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using Hidemaru;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

public class HmSelectedTextAutoHighlighterForm : Form
{
    public static HmSelectedTextAutoHighlighterForm form;
    private Timer timer;
    private const int KeepMonitoringMilliSeconds = 1000; // �Ď������ŁA������I�����u�Œ肵���v�Ƃ���ڈ����ԁB����v���X
    private const int ItvlMonitoringMilliSeconds = 300; // ��{�I�ȊĎ��́u�`�F�b�N�v�Ԋu
    private const int mustMonitoringCount = KeepMonitoringMilliSeconds / ItvlMonitoringMilliSeconds;  // ���j�^�[���ׂ��J�E���g��

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

    // �G�ۂ̃}�[�J�[��Ԃ��X�V����
    private bool TryUpdateHidemaruMarker(string text)
    {
        if (!Hm.Macro.IsExecuting)
        {
            // �e�L�X�g��I�����Ă��Ȃ��A�������́A�e�L�X�g����������
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

    // �I�����Ă���e�L�X�g���u���肵���v�Ƃ�������
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

    // �I�����Ă���e�L�X�g��Tick���Ƃɒ~�ς��邽�߂�Queue�ɂ��߂���
    private void AddQueueBufferSelectedText(String text)
    {
        // �o�b�t�@�[�ɉ�����
        bufferSelectedText.Add(text);

        // ���j�^�[���ׂ��J�E���g�����A�o�b�t�@�[�̃J�E���g������������A�擪�����菜��
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
