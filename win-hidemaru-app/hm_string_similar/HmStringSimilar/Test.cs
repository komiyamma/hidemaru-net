using System;
using System.Collections.Generic;
using System.Linq;

#if DEBUG
namespace HmStringSimilar
{
    class program
    {
        public static void Main()
        {
            Console.WriteLine("OK");
            List<string> list = new List<string>();
            list.Add("クロワッサン");
            list.Add("クリイム");
            list.Add("クレーム");
            list.Add("クライム");
            list.Add("コロネパン");
            list.Add("クリーナ");
            list.Add("リクーム");
            list.Add("クローバースタジオ");
            list.Add("クローム");
            list.Add("クラウド");
            list.Add("クリゴハン");
            list.Add("クリキントン");
            list.Add("クリームパン");
            list.Add("クリスティーナ");
            list.Add("クロノトリガー");

            Console.WriteLine(">>もしかして…ほにゃらら的な何か");
            Console.WriteLine();

            while (true)
            {
                Console.WriteLine("何かキーワードを入力してください... ( exit で終了します。)");
                var input = Console.ReadLine();
                if (input == "exit") break;

                // LevenshteinDistance.Compare
                // Ngram.CompareBigram
                // Ngram.CompareTrigram
                // Ngram.COmpareUnigram

                var candidate = (from s in list
                                 let ld = LevenshteinDistance.Compute(input, s)
                                 let bigram = Ngram.CompareBigram(input, s)
                                 orderby bigram descending
                                 orderby ld ascending
                                 select new { LD = ld, Value = s }).Take(5);

                foreach (var item in candidate) { 
                    Console.WriteLine("LD [{0}] : {1}", item.LD, item.Value);
                }

                Console.WriteLine();
            }
        }
    }
}
#endif