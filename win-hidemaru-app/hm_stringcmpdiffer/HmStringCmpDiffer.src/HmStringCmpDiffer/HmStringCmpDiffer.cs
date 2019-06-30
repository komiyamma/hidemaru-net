/*
 * This codes is licensed under CC0 1.0 Universal
 */
 
using System;
using System.Collections.Generic;
using System.Linq;
using System.Drawing;
using Path = System.Collections.Generic.IEnumerable<System.Drawing.Point>;

namespace HmStringCmpDiffer { 

    public static class StringDiffExtention
    {
        private const char Minus = '-';
        private const char Plus = '+';
        private const char Equal = '=';
        private static readonly Point ZeroPoint = new Point(x: 0, y: 0);

        public static IList<KeyValuePair<T, char>> Diff<T>(this IEnumerable<T> a,
                                                           IEnumerable<T> b)
        {
            bool swap = a.Skip(b.Count()).Any();
            IEnumerable<T> A = !swap ? a : b, B = swap ? a : b;

            int offset = A.Count() + 1;
            int delta = B.Skip(A.Count()).Count();
            int size = A.Concat(B).Count() + 3;

            int[] fp = Enumerable.Repeat<int>(-1, size).ToArray();
            var paths = Enumerable.Empty<Path>().ToList();

            for (var p = 0; fp[delta + offset] != B.Count(); ++p)
                paths.AddRange(Enumerable.Range(-p, delta + p)
                    .Concat(Enumerable.Range(delta + 1, p).Reverse())
                        .Select(k => Snake(A, B, ref fp, k, offset)).ToList()
                        .Concat(new[] { Snake(A, B, ref fp, delta, offset) }));

            return paths.MakeResultData(A, B, swap);
        }

        // ------------------------------------------------------------------------
        // 最小編集距離を求めつつ、ついでに経路情報を記録するよ∩( ・ω・)∩
        // 昔のアルゴリズムって一つの関数に複数の目的を詰め込んだ処理が多い気がする
        private static IEnumerable<Point> Snake<T>(IEnumerable<T> a,
                                                   IEnumerable<T> b,
                                                   ref int[] fp,
                                                   int k,
                                                   int offset)
        {
            var index = k + offset;
            var snake = Snake(a, b, k, Math.Max(fp[index - 1] + 1, fp[index + 1]));

            fp[index] = snake.Last().Y;
            return snake;
        }

        private static IEnumerable<Point> Snake<T>(IEnumerable<T> a,
                                                   IEnumerable<T> b,
                                                   int k, int y)
        {
            int x = y - k;
            return new[] { new Point(x, y) }
                .Concat(PointSequence(x, y)
                .TakeWhile(p => a.Skip(p.X - 1).Any() && b.Skip(p.Y - 1).Any() &&
                    a.ElementAt(p.X - 1).Equals(b.ElementAt(p.Y - 1)))).ToList();
        }

        private static IEnumerable<Point> PointSequence(int x, int y)
        {
            for (int _x = x, _y = y; ;) yield return new Point(++_x, ++_y);
        }

        private static Point OffsetX(this Point point, int offset = 1)
        {
            return new Point(x: point.X + offset, y: point.Y);
        }

        private static Point OffsetY(this Point point, int offset = 1)
        {
            return new Point(x: point.X, y: point.Y + offset);
        }

        // ---------------------------------
        // 結果データを作ります∩( ・ω・)∩
        private static IList<KeyValuePair<T, char>> MakeResultData<T>(this List<Path> paths,
                                                                      IEnumerable<T> a,
                                                                      IEnumerable<T> b,
                                                                      bool swap)
        {
            var prunedPaths = paths.Prune(swap);
            var ret = Enumerable.Empty<KeyValuePair<T, char>>().ToList();
            for (var path = prunedPaths.FirstOrDefault(); path != null;)
            {
                var tail = path.BuildCommonElementData(a, ref ret).Last();
                path = prunedPaths.BuildAddedElementData(tail, b, swap, ref ret) ??
                           prunedPaths.BuildDeletedElementData(tail, a, swap, ref ret);
            }
            return ret;
        }

        private static Path BuildAddedElementData<T>(this List<Path> paths,
                                                     Point tail,
                                                     IEnumerable<T> sequence,
                                                     bool swap,
                                                     ref List<KeyValuePair<T, char>> resultSequence)
        {
            return paths.BuildChangedElementData(tail, sequence, swap, ref resultSequence,
                                                 isAdded: true);
        }

        private static Path BuildDeletedElementData<T>(this List<Path> paths,
                                                       Point tail, IEnumerable<T> sequence,
                                                       bool swap,
                                                       ref List<KeyValuePair<T, char>> resultSequence)
        {
            return paths.BuildChangedElementData(tail, sequence, swap, ref resultSequence,
                                                 isAdded: false);
        }

        private static Path BuildChangedElementData<T>(this List<Path> paths,
                                                       Point tail,
                                                       IEnumerable<T> sequence,
                                                       bool swap,
                                                       ref List<KeyValuePair<T, char>> resultSequence,
                                                       bool isAdded)
        {
            var nextStartPoint = isAdded ? tail.OffsetY() : tail.OffsetX();
            var query = paths.Where(x => x.First().Equals(nextStartPoint));
            if (!query.Any()) return null;

            var ret = query.First();
            var index = (isAdded ? ret.First().Y : ret.First().X) - 1;
            resultSequence.Add(new KeyValuePair<T, char>
                (sequence.ElementAt(index), (isAdded ^ swap) ? Plus : Minus));
            return ret;
        }

        private static Path BuildCommonElementData<T>(this Path path,
                                                      IEnumerable<T> sequence,
                                                      ref List<KeyValuePair<T, char>> resultSequence)
        {
            int start = path.First().X;
            int count = path.Last().X - start;

            resultSequence.AddRange(Enumerable.Range(start, count)
                .Select(x => new KeyValuePair<T, char>(sequence.ElementAt(x), Equal)));

            return path;
        }

        // -------------------------------------------
        // 経路情報から余分な枝を刈ります∩( ・ω・)∩
        private static List<Path> Prune(this IEnumerable<Path> paths,
                                        bool swap)
        {
            return paths.Prune(paths.Last(), swap).ToList();
        }

        private static IEnumerable<Path> Prune(this IEnumerable<Path> paths,
                                               Path last,
                                               bool swap)
        {
            var startPoint = last.First();
            var sequence = new[] { last };

            if (startPoint.Equals(ZeroPoint)) return sequence;
            var path = paths
                .Where(p => startPoint.AdjacentPoints(swap).Contains(p.Last()))
                .Select(p => paths.Prune(p, swap))
                .Where(result => result != null)
                .FirstOrDefault();

            return path != null ? path.Concat(sequence) : null;
        }

        private static IEnumerable<Point> AdjacentPoints(this Point point, bool swap)
        {
            var x = new[] { point.OffsetX(-1), point.OffsetY(-1) };
            return !swap ? x : x.Reverse();
        }

        /*
        class Program
        {
            static void Main(String[] args)
            {
                var ret = @"「あぁ、『おっぱい』を『いっぱい』に書き換えるためには、1文字目に『い』を追加して、".Diff("これを見た人は、マシにする方法をこの記事の最後に載せておきました。");
                foreach (var r in ret)
                {
                    Console.WriteLine(r);
                }
                Console.ReadKey();
            }
        }
        */
    }
}