/*
 * HmNetPInvoke ver 1.841
 * Copyright (C) 2021 Akitsugu Komiyama
 * under the MIT License
 **/

using System;
using System.Runtime.InteropServices;

namespace HmNetPInvoke
{
    [ComVisible(true)]
    // [Guid("00000000-0000-0000-0000-000000000000")]
    [Guid("04609F5B-0889-4149-B084-E646977B2BAE")]
    public partial class HmMacroVar
    {
        static object marcroVar = null;
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

    public partial class HmMacroVar
    {
        static string myGuidLabel = "";
        static string myClassFullName = "";
        static HmMacroVar()
        {
            var h = new HmMacroVar();
            myGuidLabel = h.GetType().GUID.ToString();
            myClassFullName = h.GetType().FullName;
        }

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

        public static TMacroVar Var = new TMacroVar();
        public sealed class TMacroVar
        {
            public Object this[String var_name]
            {
                get
                {
                    ClearVar();
                    Object ret = GetVar(var_name);
                    if (ret.GetType().Name != "String")
                    {
                        return (int)ret + 0; // 確実に複製を
                    }
                    else
                    {
                        return (String)ret + ""; // 確実に複製を
                    }
                }
                set
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
                        SetVar(var_name, value);
                        ClearVar();
                    }

                    else // if (var_name.StartsWith("$")
                    {

                        String result = value.ToString();
                        SetVar(var_name, value);
                        ClearVar();
                    }
                }
            }
        }

        private static object GetVar(string var_name)
        {
            string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string myTargetDllFullPath = GetMyTargetDllFullPath(myDllFullPath);
            string myTargetClass = GetMyTargetClass(myDllFullPath);
            ClearVar();
            try
            {
                var result = Hm.Macro.Eval($@"
                    #_COM_NET_PINVOKE = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    #_COM_NET_PINVOKE_MEMBER_RESULT = member(#_COM_NET_PINVOKE, ""MacroToDll"", {var_name});
                    releaseobject(#_COM_NET_PINVOKE);
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
            return marcroVar;
        }

        private static int SetVar(string var_name, object obj)
        {
            string myDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string myTargetDllFullPath = GetMyTargetDllFullPath(myDllFullPath);
            string myTargetClass = GetMyTargetClass(myDllFullPath);
            ClearVar();
            marcroVar = obj;
            try
            {
                var result = Hm.Macro.Eval($@"
                    #_COM_NET_PINVOKE = createobject(@""{myTargetDllFullPath}"", @""{myTargetClass}"" );
                    {var_name} = member(#_COM_NET_PINVOKE, ""DllToMacro"" );
                    releaseobject(#_COM_NET_PINVOKE);
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

        private static void ClearVar()
        {
            marcroVar = null;
        }
    }


}

// #endif