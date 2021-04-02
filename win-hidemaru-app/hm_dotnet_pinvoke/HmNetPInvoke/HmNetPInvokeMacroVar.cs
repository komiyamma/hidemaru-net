/*
 * HmNetPInvoke ver 1.841
 * Copyright (C) 2021 Akitsugu Komiyama
 * under the MIT License
 **/



using System;
using System.Runtime.InteropServices;

namespace HmNetPInvoke
{
    public partial class HmMacroCOMVar {
        private const string HmMacroCOMVarInterface = "53f5efeb-1871-4044-8995-6f098dc014d1";
    }
}


namespace HmNetPInvoke
{
    // 秀丸のCOMから呼び出して、マクロ⇔COMといったように、マクロとプログラムで変数値を互いに伝搬する
    [ComVisible(true)]
    [Guid(HmMacroCOMVarInterface)]
    public partial class HmMacroCOMVar
    {
        private static object marcroVar = null;
        public object DllToMacro()
        {
            return marcroVar;
        }
        public int MacroToDll(object variable)
        {
            marcroVar = variable;
            return 1;
        }
    }

    public partial class HmMacroCOMVar
    {
        static HmMacroCOMVar()
        {
            var h = new HmMacroCOMVar();
            myGuidLabel = h.GetType().GUID.ToString();
            myClassFullName = h.GetType().FullName;
        }

        internal static void SetMacroVar(object obj)
        {
            marcroVar = obj;
        }
        internal static object GetMacroVar()
        {
            return marcroVar;
        }
        private static string myGuidLabel = "";
        private static string myClassFullName = "";

        internal static string GetMyTargetDllFullPath(string thisDllFullPath)
        {
            string myTargetClass = myClassFullName;
            string thisComHostFullPath = System.IO.Path.ChangeExtension(thisDllFullPath, "comhost.dll");
            if (System.IO.File.Exists(thisComHostFullPath))
            {
                return thisComHostFullPath;
            }

            return thisDllFullPath;
        }

        internal static string GetMyTargetClass(string thisDllFullPath)
        {
            string myTargetClass = myClassFullName;
            string thisComHostFullPath = System.IO.Path.ChangeExtension(thisDllFullPath, "comhost.dll");
            if (System.IO.File.Exists(thisComHostFullPath))
            {
                myTargetClass = "{" + myGuidLabel + "}";
            }

            return myTargetClass;
        }

        internal static object GetVar(string var_name)
        {
            string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string myTargetDllFullPath = GetMyTargetDllFullPath(myDllFullPath);
            string myTargetClass = GetMyTargetClass(myDllFullPath);
            ClearVar();
            try
            {
                var result = Hm.Macro.Eval($@"
                    #_COM_NET_PINVOKE_MACRO_VAR = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    #_COM_NET_PINVOKE_MACRO_VAR_RESULT = member(#_COM_NET_PINVOKE_MACRO_VAR, ""MacroToDll"", {var_name});
                    releaseobject(#_COM_NET_PINVOKE_MACRO_VAR);
                    #_COM_NET_PINVOKE_MACRO_VAR_RESULT = 0;
                ");
                if (result.Error != null)
                {
                    throw result.Error;
                }
            }
            catch (Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e);
            }
            return HmMacroCOMVar.marcroVar;
        }

        internal static int SetVar(string var_name, object obj)
        {
            string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string myTargetDllFullPath = GetMyTargetDllFullPath(myDllFullPath);
            string myTargetClass = GetMyTargetClass(myDllFullPath);
            ClearVar();
            HmMacroCOMVar.marcroVar = obj;
            try
            {
                var result = Hm.Macro.Eval($@"
                    #_COM_NET_PINVOKE_MACRO_VAR = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    {var_name} = member(#_COM_NET_PINVOKE_MACRO_VAR, ""DllToMacro"" );
                    releaseobject(#_COM_NET_PINVOKE_MACRO_VAR);
                ");
                if (result.Error != null)
                {
                    throw result.Error;
                }

            }
            catch (Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e);
                return 0;
            }
            return 1;
        }

        internal static void ClearVar()
        {
            HmMacroCOMVar.marcroVar = null;
        }
    }


#if BUILD_DLL
    public partial class Hm
#else
    internal partial class Hm
#endif
    {
        public static partial class Edit
        {
            static partial void SetTotalText(string text)
            {
                string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
                string myTargetDllFullPath = HmMacroCOMVar.GetMyTargetDllFullPath(myDllFullPath);
                string myTargetClass = HmMacroCOMVar.GetMyTargetClass(myDllFullPath);
                HmMacroCOMVar.ClearVar();
                HmMacroCOMVar.SetMacroVar(text);
                try
                {
                    var result = Hm.Macro.Eval($@"
                    begingroupundo;
                    selectall;
                    #_COM_NET_PINVOKE_MACRO_VAR = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    insert member(#_COM_NET_PINVOKE_MACRO_VAR, ""DllToMacro"" );
                    releaseobject(#_COM_NET_PINVOKE_MACRO_VAR);
                    endgroupundo;
                ");
                    if (result.Error != null)
                    {
                        throw result.Error;
                    }

                }
                catch (Exception e)
                {
                    System.Diagnostics.Trace.WriteLine(e);
                }
            }

            static partial void SetSelectedText(string text)
            {
                string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
                string myTargetDllFullPath = HmMacroCOMVar.GetMyTargetDllFullPath(myDllFullPath);
                string myTargetClass = HmMacroCOMVar.GetMyTargetClass(myDllFullPath);
                HmMacroCOMVar.ClearVar();
                HmMacroCOMVar.SetMacroVar(text);
                try
                {
                    var result = Hm.Macro.Eval($@"
                    if (selecting) {{
                    #_COM_NET_PINVOKE_MACRO_VAR = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    insert member(#_COM_NET_PINVOKE_MACRO_VAR, ""DllToMacro"" );
                    releaseobject(#_COM_NET_PINVOKE_MACRO_VAR);
                    }}
                ");
                    if (result.Error != null)
                    {
                        throw result.Error;
                    }

                }
                catch (Exception e)
                {
                    System.Diagnostics.Trace.WriteLine(e);
                }
            }

            static partial void SetLineText(string text)
            {
                string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
                string myTargetDllFullPath = HmMacroCOMVar.GetMyTargetDllFullPath(myDllFullPath);
                string myTargetClass = HmMacroCOMVar.GetMyTargetClass(myDllFullPath);
                HmMacroCOMVar.ClearVar();
                HmMacroCOMVar.SetMacroVar(text);
                try
                {
                    var pos = Edit.CursorPos;
                    var result = Hm.Macro.Eval($@"
                    begingroupundo;
                    selectline;
                    #_COM_NET_PINVOKE_MACRO_VAR = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    insert member(#_COM_NET_PINVOKE_MACRO_VAR, ""DllToMacro"" );
                    releaseobject(#_COM_NET_PINVOKE_MACRO_VAR);
                    moveto2 {pos.Column}, {pos.LineNo};
                    endgroupundo;
                ");
                    if (result.Error != null)
                    {
                        throw result.Error;
                    }

                }
                catch (Exception e)
                {
                    System.Diagnostics.Trace.WriteLine(e);
                }
            }

        }


        public static partial class Macro
        {
            /// <summary>
            /// 対象の「秀丸マクロ変数名」への読み書き
            /// </summary>
            /// <param name = "var_name">書き込みの場合は、変数の値</param>
            /// <returns>読み取りの場合は、対象の変数の値</returns>
            internal static TMacroVar Var = new TMacroVar();
            internal sealed class TMacroVar
            {
                public Object this[String var_name]
                {
                    get
                    {
                        return GetMethod(var_name);
                    }
                    set
                    {
                        value = SetMethod(var_name, value);
                    }
                }

                private static object SetMethod(string var_name, object value)
                {
                    if (var_name.StartsWith("#"))
                    {
                        Object result = new Object();

                        // Boolean型であれば、True:1 Flase:0にマッピングする
                        if (value is bool)
                        {
                            if ((Boolean)value == true)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = 0;
                            }
                        }

                        // まずは整数でトライ
                        int itmp = 0;
                        bool success = int.TryParse(value.ToString(), out itmp);
                        if (success == true)
                        {
                            result = itmp;
                        }
                        else
                        {
                            // 次に少数でトライ
                            double dtmp = 0;
                            success = double.TryParse(value.ToString(), out dtmp);
                            if (success)
                            {
                                result = (int)Math.Floor(dtmp);
                            }
                            else
                            {
                                result = 0;
                            }
                        }
                        HmMacroCOMVar.SetVar(var_name, value);
                        HmMacroCOMVar.ClearVar();
                    }

                    else // if (var_name.StartsWith("$")
                    {

                        String result = value.ToString();
                        HmMacroCOMVar.SetVar(var_name, value);
                        HmMacroCOMVar.ClearVar();
                    }

                    return value;
                }

                private static object GetMethod(string var_name)
                {
                    HmMacroCOMVar.ClearVar();
                    Object ret = HmMacroCOMVar.GetVar(var_name);
                    if (ret.GetType().Name != "String")
                    {
                        return (int)ret + 0; // 確実に複製を
                    }
                    else
                    {
                        return (String)ret + ""; // 確実に複製を
                    }
                }
            }
        }
    }
}

