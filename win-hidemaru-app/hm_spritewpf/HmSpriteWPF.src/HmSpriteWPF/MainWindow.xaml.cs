using System.Collections.Generic;
using System.Windows;
using System.Windows.Threading;

namespace HmSpriteWPF
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            AnalyzeArgs();

            InitializeComponent();

            SetFormAttribute();
            SetCanvasAttribute();

            // フォームを対象とするウィンドウに追従させるためのタイマー構築などなど
            ResetFollowPosition();
            SetPositionTimerAttribute();
        }

        private void SetFormAttribute()
        {
            this.Width = 1;
            this.Height = 1;
            this.Top = 0;
            this.Left = 0;

            SetFormNoActiveAttribute();
        }

        List<DispatcherTimer> timerList = new List<DispatcherTimer>();

        // やめ。
        private void StopAndClose()
        {
            // タイマーをストップ
            foreach (var t in timerList)
            {
                t.Stop();
            }
            this.Close();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ConsumeFadeTime();
        }
    }

}
