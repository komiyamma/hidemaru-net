/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;




// ★秀丸クラス
internal sealed partial class hmNETDynamicLib
{
    internal sealed partial class Hidemaru
    {
        public static List<String> AssemblyPath = new List<string>();

        public Hidemaru()
        {
            System.Diagnostics.FileVersionInfo vi = System.Diagnostics.FileVersionInfo.GetVersionInfo(strExecuteFullpath);
            _ver = 100 * vi.FileMajorPart + 10 * vi.FileMinorPart + 1 * vi.FileBuildPart + 0.01 * vi.FilePrivatePart;
            // System.Diagnostics.Trace.WriteLine(_ver);
            SetUnManagedDll();
        }

        internal sealed class ErrorMsg
        {
            public const String MethodNeed866 = "このメソッドは秀丸エディタ v8.66 正式版以降で利用可能です。";
            public const String MethodNeed873 = "このメソッドは秀丸エディタ v8.73 正式版以降で利用可能です。";
            public const String MethodNeed875 = "このメソッドは秀丸エディタ v8.75 正式版以降で利用可能です。";
            public const String MethodNeed877 = "このメソッドは秀丸エディタ v8.77 正式版以降で利用可能です。";
            public const String MethodNeed890 = "このメソッドは秀丸エディタ v8.90 正式版以降で利用可能です。";
            public const String MethodNeedOutputNotFound = "HmOutputPaneの対象関数を発見できません。";
            public const String MethodNeedOutputOperation = "HmOutputPaneへの操作中にエラーが発生しました。";
            public static readonly String NoDllBindHandle866 = strDllFullPath + "をloaddllした際の束縛変数の値を特定できません";
        }

        private static IntPtr _hWndHidemaru = IntPtr.Zero;
        public static IntPtr WindowHandle
        {
            get
            {
                if (pGetCurrentWindowHandle != null)
                {
                    // System.Diagnostics.Trace.WriteLine("自動取得");
                    IntPtr tmp = pGetCurrentWindowHandle();
                    // System.Diagnostics.Trace.WriteLine(tmp);
                    return pGetCurrentWindowHandle();

                }

                // System.Diagnostics.Trace.WriteLine("手動取得");
                _hWndHidemaru = HmWndHandleSearcher.GetCurWndHidemaru(_hWndHidemaru);
                // System.Diagnostics.Trace.WriteLine(_hWndHidemaru);
                if (_hWndHidemaru != IntPtr.Zero)
                {
                    return _hWndHidemaru;
                }

                return IntPtr.Zero;
            }
        }

        // debuginfo関数
        public static void debuginfo(params Object[] expressions)
        {
            List<String> list = new List<String>();
            foreach (var exp in expressions)
            {

                list.Add(exp.ToString());
            }

            String joind = String.Join(" ", list);
            OutputDebugStream(joind);
        }

        // バージョン。hm.versionのため。読み取り専用
        static double _ver;
        public static double version
        {
            get { return _ver; }
        }
    }
}
