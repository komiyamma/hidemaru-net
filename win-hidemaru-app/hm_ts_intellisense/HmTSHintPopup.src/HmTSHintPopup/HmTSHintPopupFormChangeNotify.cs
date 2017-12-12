/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

 using Hidemaru;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows.Forms;

// ファイルの変更を検知すると、原則的には閉じるのであるが、
// ファイルの変更直前に、秀丸マクロ側から、新たなるファイルの命令があった場合には継続することとなる。
internal partial class HmTSHintPopupForm : Form
{
    class NotifyData : ICloneable
    {
        public String strFilePath;
        public uint time;
        public bool hasData;

        // ICloneable.Cloneの実装
        public object Clone()
        {
            return MemberwiseClone();
        }
    }

    // 毎チックのデータ。最終２つだけ格納
    List<NotifyData> listNotifyData = new List<NotifyData>();
    const int listNotifyDataCapacity = 2;

    // 継続データ。
    static NotifyData cointinueData = new NotifyData();

    // 継続データの設定
    public static void SetContinueNotify()
    {
        lock (cointinueData)
        {
            try
            {
                var filename = Hm.Edit.FilePath ?? "";
                cointinueData = new NotifyData { hasData = true, strFilePath = filename, time = timeGetTime() };
            }
            catch (Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
                System.Diagnostics.Trace.WriteLine(e.StackTrace);
            }
        }
    }

    private bool isClosed = false;
    public bool IsClosed()
    {
        return isClosed;
    }

    // ここがTickごとに実行される
    private bool TimerTick_Notify(String strCurFileName)
    {
        bool ret = _TimerTick_Notify(strCurFileName);
        cointinueData.hasData = false;
        return ret;
    }

    private bool _TimerTick_Notify(String strCurFileName)
    {
        // 要素が０なら比較対象が無いので加えるだけ
        if (listNotifyData.Count == 0)
        {
            NotifyData data = new NotifyData { strFilePath = strCurFileName, time = timeGetTime() };

            listNotifyData.Add(data);
        }
        else
        {
            lock (cointinueData)
            {
                // 毎チックで蓄積していた最後のデータと、現在のファイルを比較する
                NotifyData last = listNotifyData[listNotifyData.Count - 1]; // 最後の要素

                // ファイル名が違っているならファイルが変化した
                if (last.strFilePath != strCurFileName)
                {
                    // System.Diagnostics.Trace.WriteLine("ファイル名が変わった");

                    // 新たな蓄積データの作成
                    NotifyData curData = new NotifyData { strFilePath = strCurFileName, time = timeGetTime() };

                    // 加えるが、キャパシティより大きいぶんは削除(要するにキュー)
                    listNotifyData.Add(curData);
                    if (listNotifyData.Count > listNotifyDataCapacity)
                    {
                        listNotifyData.RemoveAt(0);
                    }

                    // コンテニューデータがあって、ファイルパスも同じ
                    if (cointinueData.hasData && cointinueData.strFilePath == curData.strFilePath)
                    {
                        // コンティニューデータが格納されてから、現在まで0.5秒以内
                        if (curData.time - cointinueData.time < 500)
                        {
                            // (また変化したと思わないように)コンティニューデータを蓄積。キャパシティより大きいぶんは削除(要するにキュー)
                            listNotifyData.Add((NotifyData)cointinueData.Clone());
                            if (listNotifyData.Count > listNotifyDataCapacity)
                            {
                                listNotifyData.RemoveAt(0);

                            }

                            // 次ステップ以降を継続してOKだ。
                            return true;
                        }
                    }

                    System.Diagnostics.Trace.WriteLine("閉じる");
                    this.Close();

                    // 次ステップ以降は処理するだけ無駄
                    return false;
                }
            } // lock
        }

        // ファイルは同じである、次ステップ以降の処理も継続してOKだ。
        return true;
    }

    [DllImport("winmm.dll", EntryPoint = "timeGetTime")]
    public static extern uint timeGetTime();

}
