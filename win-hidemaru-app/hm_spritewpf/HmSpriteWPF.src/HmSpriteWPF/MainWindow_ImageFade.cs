/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Windows;
using System.Windows.Threading;

namespace HmSpriteWPF
{
    public partial class MainWindow : Window
    {
        // フェイド関係を更新するタイマー
        DispatcherTimer timer_Fade;

        // 残りタイム３種(フェイドイン・表示維持・フェイドアウト。いずれもミリ秒)
        int remainFadeInTime;
        int remainFadeKeepTime;
        int remainFadeOutTime;

        // タイマーを一気に消費。フェイドアウト以外は全部消費(マウスでクリックしたり用)
        private void ConsumeFadeTime()
        {
            this.remainFadeInTime = -1;
            this.remainFadeKeepTime = -1;
        }

        // フェイド用のタイマー構築。
        private void SetImageFadeTimer()
        {
            remainFadeInTime = options.spriteFadeInTime;
            remainFadeKeepTime = options.spriteFadeKeepTime;
            remainFadeOutTime = options.spriteFadeOutTime;

            this.timer_Fade = new DispatcherTimer();
            timer_Fade.Interval = new TimeSpan(0, 0, 0, 0, 33);
            timer_Fade.Tick += Timer_Fade_Tick;
            timerList.Add(timer_Fade);
            timer_Fade.Start();
        }

        private void Timer_Fade_Tick(object sender, EventArgs e)
        {
            // System.Console.WriteLine("in;" + remainFadeInTime);
            // System.Console.WriteLine("out;" + remainFadeOutTime);

            // 全ての時間を使い果たしたら、タイマー止めて自分自身をクローズ
            if (this.remainFadeOutTime < 0)
            {
                StopAndClose();
                return;
            }

            // まずはフェイドインタイマーの消費
            remainFadeInTime -= timer_Fade.Interval.Milliseconds;

            // フェイドインタイマーが残っていたら
            if (this.remainFadeInTime > 0)
            {
                // 不透明度へと反映。 フェイドインタイマーの残っている割合は、そのまま「透明度」と言える。
                // 不透明度にするには「1.0-透明度」
                this.Opacity = 1.0 - ((double)remainFadeInTime / (double)options.spriteFadeInTime);
                return;
            }

            // 完全表示のキープ状態が残っている
            remainFadeKeepTime -= timer_Fade.Interval.Milliseconds;
            if (this.remainFadeKeepTime > 0)
            {
                // 完全不透明
                this.Opacity = 1.0;
                return;
            }

            // フェイドアウトタイムが残っている
            remainFadeOutTime -= timer_Fade.Interval.Milliseconds;
            if (this.remainFadeOutTime > 0)
            {
                // フェイドアウトタイムが残っている割合がそのまま不透明度となる
                this.Opacity = 0.0 + ((double)remainFadeOutTime / (double)options.spriteFadeOutTime);
            }
        }
    }
}
