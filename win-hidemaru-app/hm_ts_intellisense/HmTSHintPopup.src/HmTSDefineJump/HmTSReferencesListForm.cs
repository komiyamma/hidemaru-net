/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

 using Hidemaru;
using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;

// これらはアクティブ化しないための特殊な施策。
partial class HmTSDefineJumpForm
{

    private static void ClearReferenceResult()
    {
        Hm.Macro.Var["#RTN_REFERENCES_MAX"] = 0;
    }

    // 秀丸用のアウトプットパネル。簡易
    private static void SetReferenceResult(int ix, JsonDataReferencesBodyRef r)
    {
        //ローカルファイルのパスを表すURI
        string uriPath = r.file;

        //Uriオブジェクトを作成する
        Uri u = new Uri(uriPath);

        //Windows形式のパス表現に変換する
        string winPath = u.LocalPath + Uri.UnescapeDataString(u.Fragment);


        String text = winPath + "(" + r.start.line + ":" + (r.start.offset) + "):\t" + r.lineText;

        try
        { 
            Hm.Macro.Var["$RTN_REFERENCES[" + ix + "]" ] = text;
            Hm.Macro.Var["#RTN_REFERENCES_MAX"] = ix + 1;
        }
        catch (Exception e)
        {
            Trace.WriteLine(e.Message);
        }
    }
    private static bool AnalyzeRecievedDataAsReferences(string recieve_data)
    {
        // 必ずjson形式で標準入力に入ってくる
        var serializer = new DataContractJsonSerializer(typeof(JsonDataReferences));
        using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(recieve_data)))
        {
            var data = (JsonDataReferences)serializer.ReadObject(ms);

            // commandとしてquickinfoを要求
            if (data.command == "references")
            {
                // 結果を伴うようなら
                if (data.success)
                {
                    var refs = data.body.refs;

                    if (refs.Length == 0)
                    {
                    } else {
                        for (int ix=0; ix < refs.Length; ix++)
                        {
                            var r = refs[ix];
                            SetReferenceResult(ix, r);
                        }
                    }
                }
                // それ以外はクリア
                else
                {
                }

                isDataAnalyzed = true;

                return true;
            }
        }

        return false;
    }

    // tsserverは結果はJsonで来るため。
    [DataContract]
    class JsonDataReferences
    {
        [DataMember]
        public int @seq;

        [DataMember]
        public string @type;

        [DataMember]
        public string @command;

        [DataMember]
        public bool @success;

        [DataMember]
        public JsonDataReferencesBody @body;

    }

    // definitionの時のbody内容。他にもあるが、この程度で良いだろう。
    [DataContract]
    class JsonDataReferencesBody
    {
        [DataMember]
        public JsonDataReferencesBodyRef[] @refs;

        [DataMember]
        public String @symbolName;

        [DataMember]
        public int @symbolStartOffset;

        [DataMember]
        public String symbolDisplayString;
    }

    [DataContract]
    class JsonDataReferencesBodyRef
    {
        [DataMember]
        public String @file;

        [DataMember]
        public JsonDataReferencesBodyRefsPos @start;

        [DataMember]
        public JsonDataReferencesBodyRefsPos @end;

        [DataMember]
        public String @lineText;

        [DataMember]
        public bool @isWriteAccess;

        [DataMember]
        public bool @isDefinition;
    }

    [DataContract]
    class JsonDataReferencesBodyRefsPos
    {
        [DataMember]
        public int @line;

        [DataMember]
        public int @offset;
    }


}


