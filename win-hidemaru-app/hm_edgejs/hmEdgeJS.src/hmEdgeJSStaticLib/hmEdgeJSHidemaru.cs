﻿/* 
 * Copyright (c) 2016-2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;



// ★秀丸クラス
public sealed partial class hmEdgeJSDynamicLib
{
    public sealed partial class Hidemaru
    {
        static Hidemaru()
        {
            System.Diagnostics.FileVersionInfo vi = System.Diagnostics.FileVersionInfo.GetVersionInfo(strExecuteFullpath);
            _ver = 100 * vi.FileMajorPart + 10 * vi.FileMinorPart + 1 * vi.FileBuildPart + 0.01 * vi.FilePrivatePart;

            SetUnManagedDll();
        }

        public sealed class ErrorMsg
        {
            public const String MethodNeed866 = "このメソッドは秀丸エディタ v8.66 正式版以降で利用可能です。";
            public const String MethodNeed873 = "このメソッドは秀丸エディタ v8.73 正式版以降で利用可能です。";
            public const String MethodNeed875 = "このメソッドは秀丸エディタ v8.75 正式版以降で利用可能です。";
            public const String MethodNeed890 = "このメソッドは秀丸エディタ v8.90 正式版以降で利用可能です。";
            public static readonly String NoDllBindHandle866 = strDllFullPath + "をloaddllした際の束縛変数の値を特定できません";
        }

        // debuginfo関数
        public static void debuginfo(Object obj)
        {
            OutputDebugStream(obj.ToString());
        }

        // バージョン。hm.versionのため。読み取り専用
        static double _ver;
        public static double version
        {
            get { return _ver; }
        }

    }
}