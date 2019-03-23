/* 
 * Copyright (c) 2019 Akitsugu Komiyama
 * under the MIT License
 */

using System.IO;
using System.Runtime.Serialization.Json;
using System.Text;


namespace HmGoogleLangTranslate
{
    internal static class JsonUtilityExtension
    {
        public static string ToJson<T>(this T obj) where T : class
        {
            var serializer = new DataContractJsonSerializer(typeof(T));

            using (var stream = new MemoryStream())
            {
                serializer.WriteObject(stream, obj);
                return Encoding.UTF8.GetString(stream.ToArray());
            }

        }

        public static T FromJson<T>(this T obj, string json) where T : class
        {
            using (var stream = new MemoryStream(Encoding.UTF8.GetBytes(json)))
            {
                var serializer = new DataContractJsonSerializer(typeof(T));
                return serializer.ReadObject(stream) as T;
            }
        }

    }
}
