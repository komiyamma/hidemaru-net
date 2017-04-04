using System.Collections.Generic;

namespace HtmlAgilityPack
{
    internal static class Utilities
    {
        public static TValue GetDictionaryValueOrNull<TKey, TValue>(this Dictionary<TKey, TValue> dict, TKey key) where TKey : class
        {
            return dict.ContainsKey(key) ? dict[key] : default(TValue);
        }
    }
}
