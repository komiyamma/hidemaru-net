using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HmEditorFaqConverter
{
    class Program
    {
        static void Main(string[] args)
        {
            string[] files = Directory.GetFiles(@".", "*.html");
            foreach (var filepath in files)
            {
                var sjis = Encoding.GetEncoding(932);
                try
                {
                    var html = System.IO.File.ReadAllText(filepath, sjis);

                    HtmlAgilityPack.HtmlDocument htmlDoc = new HtmlAgilityPack.HtmlDocument();
                    htmlDoc.LoadHtml(html);

                    // bodyの中身だけ取得して
                    HtmlAgilityPack.HtmlNode bodyNode = htmlDoc.DocumentNode.SelectSingleNode("//body");
                    // Console.WriteLine(bodyNode.InnerHtml);

                    if (bodyNode.OuterHtml != null)
                    {
                        // BOM付きのutf8円コーディイングで保存しておくことで、2回目に実行したとしてもsjis誤爆で再変換なんてことはなくなる(上のsjis指定が自動的にutf8へと強制修正されるため)
                        System.IO.File.WriteAllText(filepath, bodyNode.InnerHtml, new UTF8Encoding(true));
                    }
                } catch(Exception e)
                {
                    Console.WriteLine(e.Message);
                }

            }
        }
    }
}
