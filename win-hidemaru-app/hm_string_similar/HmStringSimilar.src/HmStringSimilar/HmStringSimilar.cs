/*
 * This codes is licensed under CC0 1.0 Universal
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HmStringSimilar
{
    /// <summary>
    /// レーベンシュタイン距離
    /// </summary>
    public static class LevenshteinDistance
    {
        public static int Compute(string s, string t)
        {
            if (s == null) throw new ArgumentNullException("s");
            if (t == null) throw new ArgumentNullException("t");

            int x = s.Length;
            int y = t.Length;

            if (x == 0) return y;
            if (y == 0) return x;

            int[,] d = new int[x + 1, y + 1];

            for (int i = 0; i <= x; d[i, 0] = i++) ;
            for (int j = 0; j <= y; d[0, j] = j++) ;

            int cost = default(int);
            for (int i = 1; i <= x; i++)
            {
                for (int j = 1; j <= y; j++)
                {
                    cost = (s.Substring(i - 1, 1) != t.Substring(j - 1, 1) ? 1 : 0);
                    d[i, j] = Math.Min(Math.Min(d[i - 1, j] + 1, d[i, j - 1] + 1)
                                     , d[i - 1, j - 1] + cost);
                }
            }
            return d[x, y];
        }
    }


    /// <summary>
    /// N-gram
    /// </summary>
    public class Ngram
    {
        /// <summary>
        /// Unigram
        /// </summary>
        /// <param name="s"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        public static decimal CompareUnigram(string s, string t)
        {
            return Compare(1, s, t);
        }

        /// <summary>
        /// Bigram
        /// </summary>
        /// <param name="s"></param>
        /// <param name="t"></param>
        /// <returns>
        /// </returns>
        public static decimal CompareBigram(string s, string t)
        {
            return Compare(2, s, t);
        }

        /// <summary>
        /// Trigram
        /// </summary>
        /// <param name="s"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        public static decimal CompareTrigram(string s, string t)
        {
            return Compare(3, s, t);
        }

        private static decimal Compare(int n, string s, string t)
        {
            if (s == null) throw new ArgumentNullException("s");
            if (t == null) throw new ArgumentNullException("t");

            var noise = new List<string>();
            for (int i = 0; i < s.Length - (n - 1); i++)
            {
                var ngitem = s.Substring(i, n);
                if (!noise.Contains(ngitem)) { noise.Add(ngitem); }
            }
            if (noise.Count == 0) return 0;

            int coincidence = 0;
            for (int i = 0; i < t.Length - (n - 1); i++)
            {
                var ngitem = t.Substring(i, n);
                if (noise.Contains(ngitem)) { coincidence++; }
            }
            return (decimal)coincidence / noise.Count;
        }
    }

}
