/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace HmSpriteWPF
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        Canvas canvas;

        private void SetCanvasAttribute()
        {
            canvas = new Canvas();
            this.Content = canvas;

            SetImageAttribute();
        }

        Image image;
        private void SetImageAttribute()
        {
            // ファイル名や残り時間をoptionを元にフィールドへ
            var spriteFileName = options.spriteFileName;

            image = new Image();
            using (
                Stream stream = new FileStream(
                    options.spriteFileName,
                    FileMode.Open, FileAccess.Read, FileShare.ReadWrite | FileShare.Delete
                )
             )
            {
                // ロックしないように指定したstreamを使用する。
                BitmapDecoder decoder = BitmapDecoder.Create(
                    stream,
                    BitmapCreateOptions.None, // この辺のオプションは適宜
                    BitmapCacheOption.Default // これも
                );
                BitmapSource bmp = new WriteableBitmap(decoder.Frames[0]);
                bmp.Freeze();

                image.Source = bmp;
            }
            image.Visibility = Visibility.Hidden;
            Canvas.SetLeft(image, 0);
            Canvas.SetTop(image, 0);
            canvas.Children.Add(image);

            SetImageFadeTimer();
        }
    }
}
