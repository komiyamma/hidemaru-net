/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Windows;

namespace HmSpriteWPF
{
    // SpriteFormクラスに渡すための、オプションのインターフェイス
    public interface ISpriteOptions
    {
        IntPtr wndHandle { get; }
        String spriteFileName { get; }
        String spriteType { get; }
        int spriteFadeInTime { get; }
        int spriteFadeKeepTime { get; }
        int spriteFadeOutTime { get; }
    }

    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        private ArgsSpriteOptions options;

        // 独自アルゴリズムを組めるように。今は単純に出し入れ可能なプロパティのみ。
        class ArgsSpriteOptions : ISpriteOptions
        {
            public IntPtr wndHandle { get; set; }
            public string spriteFileName { get; set; }
            public string spriteType { get; set; }
            public int spriteFadeInTime { get; set; }
            public int spriteFadeKeepTime { get; set; }
            public int spriteFadeOutTime { get; set; }
        }

        // コマンドライン引数の分析
        private void AnalyzeArgs()
        {
            var args = Environment.GetCommandLineArgs();

            try
            {
                if (args.Length >= 6)
                {
                    // 引数をオプションに
                    options = new ArgsSpriteOptions()
                    {
                        wndHandle = new IntPtr(Int64.Parse(args[1])),
                        spriteFileName = args[2],
                        spriteType = args[3],
                        spriteFadeInTime = Int32.Parse(args[4]),
                        spriteFadeKeepTime = Int32.Parse(args[5]),
                        spriteFadeOutTime = Int32.Parse(args[6])
                    };

                }
                else
                {
                    // 引数をオプションに
                    options = new ArgsSpriteOptions
                    {
                        wndHandle = FindWindow("Tenshouki95", IntPtr.Zero),
                        spriteFileName = "haru.png",
                        spriteType = "png",
                        spriteFadeInTime = 400,
                        spriteFadeKeepTime = 2000,
                        spriteFadeOutTime = 400
                    };

                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}
