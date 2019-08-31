using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.IO;
using Hidemaru;

namespace HmMakePngSeqReport
{
    public class PngTool
    {
        // ２つのファイルを比較（まぁまぁ早いが２比較を繰り返すと時間がかかるのでこれは使わない）
        public static bool Compare5(Image image1, Image image2)
        {
            Bitmap img1 = (Bitmap)image1;
            Bitmap img2 = (Bitmap)image2;

            //高さが違えばfalse
            if (img1.Width != img2.Width || img1.Height != img2.Height) return false;
            //BitmapData取得
            BitmapData bd1 = img1.LockBits(new Rectangle(0, 0, img1.Width, img1.Height), System.Drawing.Imaging.ImageLockMode.ReadOnly, img1.PixelFormat);
            BitmapData bd2 = img2.LockBits(new Rectangle(0, 0, img2.Width, img2.Height), System.Drawing.Imaging.ImageLockMode.ReadOnly, img2.PixelFormat);
            //スキャン幅が違う場合はfalse
            if (bd1.Stride != bd2.Stride)
            {
                //ロックを解除
                img1.UnlockBits(bd1);
                img2.UnlockBits(bd2);
                return false;
            }
            int bsize = bd1.Stride * img1.Height;
            byte[] byte1 = new byte[bsize];
            byte[] byte2 = new byte[bsize];
            //バイト配列にコピー
            Marshal.Copy(bd1.Scan0, byte1, 0, bsize);
            Marshal.Copy(bd2.Scan0, byte2, 0, bsize);
            //ロックを解除
            img1.UnlockBits(bd1);
            img2.UnlockBits(bd2);

            //MD5ハッシュを取る
            System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();

            byte[] hash1 = md5.ComputeHash(byte1);
            byte[] hash2 = md5.ComputeHash(byte2);

            //ハッシュを比較
            return hash1.SequenceEqual(hash2);
        }
    }

    public partial class MakePngSeqReport
    {

        // イメージを全部かかえるとメモリが大変かもしれないので、MD5で格納
        private static byte[] GetImageMD5Hash(Image image1)
        {
            Bitmap img1 = (Bitmap)image1;
            //BitmapData取得
            BitmapData bd1 = img1.LockBits(new Rectangle(0, 0, img1.Width, img1.Height), System.Drawing.Imaging.ImageLockMode.ReadOnly, img1.PixelFormat);

            int bsize = bd1.Stride * img1.Height;
            byte[] byte1 = new byte[bsize];

            //バイト配列にコピー
            Marshal.Copy(bd1.Scan0, byte1, 0, bsize);

            //ロックを解除
            img1.UnlockBits(bd1);

            //MD5ハッシュを取る
            System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();

            byte[] hash1 = md5.ComputeHash(byte1);
            return hash1;
        }


        // ターゲットのファイルフルパスのベースパターンと、数値部分の桁数を得る
        private static (string filepbase, int num_of_digits) GetTargetPngFilePatternInfo(String filefullpath)
        {
            // 基本パターンを得る。ただし、.png の前に数値を含んでいること
            var m = Regex.Match(filefullpath, @"(.+?)(\d+)(\.png)$", RegexOptions.IgnoreCase);
            if (!m.Success)
            {
                return ("", 0);
            }

            // 数値
            string strnum = m.Groups[2].Value;
            // System.Console.WriteLine(strnum);
            int length = strnum.Length;
            int inum = 0;
            bool success = int.TryParse(strnum, out inum);
            if (!success)
            {
                return ("", 0);
            }

            // 数値と桁数。一般的にPNGの連番は桁数を０埋めで維持したまま、番号が増えていくため。
            return (m.Groups[1].Value, length);
        }

        // ファイルのベースパターンと、桁数と、現在のINDEXを指定すると、存在するファイルを返す（０埋めしてるしてない両方探す）
        private static (string filename, string key) GetTargetIndexFileName(string filebase, int num_of_digits, int icurrent_ix)
        {
            string key1 = icurrent_ix.ToString();
            string name1 = filebase + key1 + ".png";

            if (File.Exists(name1))
            {
                return (name1, key1);
            }

            int sub_length = num_of_digits - icurrent_ix.ToString().Length;
            if (sub_length < 0)
            {
                sub_length = 0;
            }
            string key2 = (new string('0', sub_length)).ToString() + icurrent_ix.ToString(); // 与えられた桁数で数値を埋める。Formatで0埋めした方が自然かも
            string name2 = filebase + key2 + ".png";

            if (File.Exists(name2))
            {
                return (name2, key2);
            }

            return ("", "");
        }

        // 数値部分をキーに、数値を画像のMD5ハッシュを値に
        private static Dictionary<int, (string, string, byte[])> dic = new Dictionary<int, (string, string, byte[])>();

        // 辞書に納入する。
        private static void MakeDictionary(string target_filename)
        {
            // 指定の代表pngファイルから、ベースとなるパターンと数値部分の桁数を得る
            (string filebase, int num_of_digits) = GetTargetPngFilePatternInfo(target_filename);

            // パターンを満たしていないようなら、ダメ
            if (String.IsNullOrEmpty(filebase))
            {
                return;
            }

            int error_cnt = 0;
            for (int ix = 0; ix < 9999; ix++)
            {
                // 対象のベースパターンと、数値桁数、現在の数値から、「存在する実際のファイルを探して」ファイル名を返す
                (string filename, string key) = GetTargetIndexFileName(filebase, num_of_digits, ix);
                if (String.IsNullOrEmpty(filename))
                {
                    error_cnt++;
                    // 多少の連番飛びは許すが、飛びすぎは変なので、ファイルが無いというエラーが嵩むようなら終了
                    if (error_cnt > 20)
                    {
                        // System.Console.WriteLine("10カウントで終了");
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }

                // System.Console.WriteLine(filename);
                var image = Image.FromFile(filename);
                var md5hash = GetImageMD5Hash(image);

                // ix をキーにして追加していく
                dic.Add(ix, (key, filename, md5hash));
            }

        }

        // 重複状態を解決する。後ろに前と同じものがあれば、上書き
        private static void DeduDictionary()
        {
            var keys = dic.Keys;
            var key_list = keys.ToList();
            for (int i = 0; i < key_list.Count; i++)
            {
                for (int j = i + 1; j < key_list.Count; j++)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    // 比較したい番号
                    int num1 = key_list[i];
                    int num2 = key_list[j];

                    if (dic[num1].Item1 == dic[num2].Item1)
                    {
                        continue;
                    }

                    // ２つはハッシュが等しい
                    if (dic[num1].Item3.SequenceEqual(dic[num2].Item3))
                    {
                        // 後ろのものを前のもので代用する
                        dic[num2] = dic[num1];
                    }
                }
            }
        }

        // レポートとして出す
        private static void PrintResult()
        {
            var keys = dic.Keys;
            var key_list = keys.ToList();
            key_list.Sort();
            for (int i = 0; i < key_list.Count; i++)
            {
                // System.Console.WriteLine("------------------------------------");
                // System.Console.Write(i + " ");
                int num = key_list[i];
                var items = dic[num];
                var basename = Path.GetFileName(items.Item2);
                System.Console.WriteLine($"\"{basename}\"");
                // System.Console.WriteLine(items.Item1 + "/" + items.Item2 + "/" + items.Item3.ToString());
                // System.Console.WriteLine("------------------------------------");
            }
        }

        private static string OpenFileDialog()
        {
            //OpenFileDialogクラスのインスタンスを作成
            OpenFileDialog ofd = new OpenFileDialog();

            //はじめのファイル名を指定する
            //はじめに「ファイル名」で表示される文字列を指定する
            ofd.FileName = "default.png";
            //はじめに表示されるフォルダを指定する
            //指定しない（空の文字列）の時は、現在のディレクトリが表示される
            ofd.InitialDirectory = @".\";
            //[ファイルの種類]に表示される選択肢を指定する
            //指定しないとすべてのファイルが表示される
            ofd.Filter = "PNGファイル(*.png)|*.png|すべてのファイル(*.*)|*.*";
            //[ファイルの種類]ではじめに選択されるものを指定する
            //2番目の「すべてのファイル」が選択されているようにする
            ofd.FilterIndex = 1;
            //タイトルを設定する
            ofd.Title = "開くファイルを選択してください";
            //ダイアログボックスを閉じる前に現在のディレクトリを復元するようにする
            ofd.RestoreDirectory = true;
            //存在しないファイルの名前が指定されたとき警告を表示する
            //デフォルトでTrueなので指定する必要はない
            ofd.CheckFileExists = true;
            //存在しないパスが指定されたとき警告を表示する
            //デフォルトでTrueなので指定する必要はない
            ofd.CheckPathExists = true;

            //ダイアログを表示する
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                //OKボタンがクリックされたとき、選択されたファイル名を表示する
                Console.WriteLine(ofd.FileName);
                return ofd.FileName;
            }

            return "";
        }

        [STAThread]
        static void Main(string[] args)
        {
            string target_filename = "";

            if (args.Length == 0)
            {
                target_filename = OpenFileDialog();

            }
            else
            {
                target_filename = args[0];
            }

            if (String.IsNullOrEmpty(target_filename))
            {
                return;
            }

            MakeDictionary(target_filename);
            DeduDictionary();
            PrintResult();
        }

    }

    public partial class MakePngSeqReport
    {
        // レポートとして出す
        private static List<String> GetResultStringList()
        {
            List<String> result = new List<string>();

            var keys = dic.Keys;
            var key_list = keys.ToList();
            key_list.Sort();
            for (int i = 0; i < key_list.Count; i++)
            {
                int num = key_list[i];
                var items = dic[num];
                var basename = Path.GetFileName(items.Item2);
                result.Add(basename);
            }

            return result;
        }

        // 秀丸からの読み取り口
        public static IntPtr GetAnimationList()
        {
            dic.Clear();

            String target_filename = OpenFileDialog();

            if (String.IsNullOrEmpty(target_filename))
            {
                return (IntPtr)1;
            }

            MakeDictionary(target_filename);
            DeduDictionary();
            var result_list = GetResultStringList();
            // ダブルコーテーションで挟む
            result_list = result_list.ConvertAll((s) => { return $"\"{s}\""; });

            // 改行しつつ...
            Hm.Macro.Var["$RESULT_LIST"] = String.Join("\n", result_list);
            return (IntPtr)1;
        }
    }
}