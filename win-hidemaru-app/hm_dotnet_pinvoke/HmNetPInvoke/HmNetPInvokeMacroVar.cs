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
        public const string HmMacroCOMVarInterface = "04609F5B-0889-4149-B084-E646977B2BAE"; // ★dll単位で新たなGUIDを与えること
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

        private static string myGuidLabel = "";
        private static string myClassFullName = "";

        private static string GetMyTargetDllFullPath(string thisDllFullPath)
        {
            string myTargetClass = myClassFullName;
            string thisComHostFullPath = System.IO.Path.ChangeExtension(thisDllFullPath, "comhost.dll");
            if (System.IO.File.Exists(thisComHostFullPath))
            {
                return thisComHostFullPath;
            }

            return thisDllFullPath;
        }

        private static string GetMyTargetClass(string thisDllFullPath)
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

        public static partial class Macro
        {
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

