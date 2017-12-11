/*
 * Copyright (C) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

using Hidemaru;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Xml.Serialization;
using System.IO;
using System.Threading;

namespace HmPochiTtoNaa
{
    [Serializable]
    public class DataType
    {
        public Int64 key;
        public Int64 x;
        public Int64 y;
        public Int64 hidemaruhandle;
        public String filename2;
        public Int64 splitmode;
    };

    [Serializable]
    public class DataTypeList
    {
        public List<DataType> list = new List<DataType>();
    }

    public class HmPochiTtoNaa
    {
        static DataTypeList dtl = new DataTypeList();

        private static String GetXmlName()
        {
            string fullpath = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string dir = System.IO.Path.GetDirectoryName(fullpath);
            string name = System.IO.Path.GetFileNameWithoutExtension(fullpath);
            return System.IO.Path.Combine(dir, name + ".xml");
        }

        public static IntPtr FindLocation(IntPtr key)
        {
            try
            {
                // クリア
                dtl.list.Clear();

                DataType d = new DataType();
                d.key = key.ToInt64();

                using (Mutex mut = new Mutex(false, "HmPochiTtoNaa"))
                {
                    Hm.Macro.Var["#RTN_key"] = (Int32)(-1);

                    if (mut.WaitOne(500))
                    {
                        try
                        {
                            dtl = LoadXmlData(dtl);
                        }
                        catch (Exception ex1)
                        {
                            Trace.WriteLine("LoadXmlDataの失敗");
                        }

                        Int32 exist = dtl.list.FindIndex((elem) => elem.key == d.key);
                        if (exist != -1)
                        {
                            d = dtl.list[exist];
                            Hm.Macro.Var["#RTN_key"] = (Int32)d.key;
                            Hm.Macro.Var["#RTN_x"] = (Int32)d.x;
                            Hm.Macro.Var["#RTN_y"] = (Int32)d.y;
                            Hm.Macro.Var["#RTN_hidemaruhandle"] = new IntPtr(d.hidemaruhandle);
                            Hm.Macro.Var["#RTN_splitmode"] = (Int32)d.splitmode;
                            Hm.Macro.Var["$RTN_filename2"] = d.filename2;
                        }

                        mut.ReleaseMutex();

                        if (exist != -1) { 
                            return (IntPtr)1;
                        } else
                        {
                            return (IntPtr)0;
                        }
                    }
                }

            }
            catch (Exception e)
            {
                Trace.WriteLine(e.Message);
                Trace.WriteLine(e.StackTrace);
                Trace.WriteLine(e.Source);
                Trace.WriteLine(e.TargetSite);
            }

            return (IntPtr)0;
        }

        public static IntPtr StockLocation(IntPtr key)
        {
            try
            {
                // クリア
                dtl.list.Clear();

                DataType d = new DataType();
                d.key = key.ToInt64();

                d.x = (Int64)(dynamic)Hm.Macro.Var["x"];
                d.y = (Int64)(dynamic)Hm.Macro.Var["y"];

                d.hidemaruhandle = (Int64)(dynamic)Hm.Macro.Var["hidemaruhandle(0)"];
                d.filename2 = (String)Hm.Macro.Var["filename2"];
                d.splitmode = (Int64)(dynamic)Hm.Macro.Var["splitmode"];

                using (Mutex mut = new Mutex(false, "HmPochiTtoNaa"))
                {
                    if (mut.WaitOne(500))
                    {
                        try
                        {
                            dtl = LoadXmlData(dtl);
                        }
                        catch (Exception ex1)
                        {
                            Trace.WriteLine("LoadXmlDataの失敗");
                        }

                        Int32 exist = dtl.list.FindIndex((elem) => elem.key == d.key);
                        if (exist != -1)
                        {
                            dtl.list.RemoveAt(exist);
                        }
                        dtl.list.Add(d);

                        try
                        {
                            SaveXmlData(dtl);
                        }
                        catch (Exception ex2)
                        {
                            Trace.WriteLine("SaveXmlDataの失敗");
                        }
                    }
                }

                // クリア
                dtl.list.Clear();

                /*
                Trace.WriteLine(d.key);
                Trace.WriteLine(d.x);
                Trace.WriteLine(d.y);
                Trace.WriteLine(d.key);
                Trace.WriteLine(d.hidemaruhandle);
                Trace.WriteLine(d.filename2);
                Trace.WriteLine(d.splitmode);
                */
            }
            catch (Exception e)
            {
                Trace.WriteLine(e.Message);
                Trace.WriteLine(e.StackTrace);
                Trace.WriteLine(e.Source);
                Trace.WriteLine(e.TargetSite);
            }
            return (IntPtr)1;
        }

        private static DataTypeList LoadXmlData(DataTypeList dtl)
        {
            String xmlFilePath = GetXmlName();
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(DataTypeList));
                using (StreamReader sr = new StreamReader(xmlFilePath))
                {
                    return (DataTypeList)serializer.Deserialize(sr);
                }
            }
            catch (Exception e)
            {

            }
            return new DataTypeList();
        }

        private static void SaveXmlData(DataTypeList dtl)
        {
            String xmlFilePath = GetXmlName();
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(DataTypeList));
                using (StreamWriter sw = new StreamWriter(xmlFilePath, false, new System.Text.UTF8Encoding(false)))
                {
                    serializer.Serialize(sw, dtl);
                    sw.Close();
                }
            }
            catch (Exception e)
            {
                Trace.WriteLine(e.GetType());
                Trace.WriteLine(e.Message);
            }
        }

        public static IntPtr GetkeyCode()
        {
            Hm.Macro.Eval(@"
	
	            ##_ch = 0x30; // デフォルトは0x39。キーボード以外(メニューなどからマクロ実行)のため
                ##RET_ch = 0;
	            while (##_ch <= 0x39){
		            if (iskeydown( ##_ch )){
			            ##RET_ch = ##_ch;
                        break;
		            }
		            ##_ch = ##_ch + 1;
	            }
            ");

            return new IntPtr((dynamic)Hm.Macro.Var["##RET_ch"]);
        }

    }
}
