using System;
using System.Runtime.InteropServices;



// ★秀丸クラス
public sealed partial class hmRbDynamicLib
{
    public sealed partial class Hidemaru
    {
        public Hidemaru()
        {
            SetUnManagedDll();
        }

        sealed class ErrorMsg
        {
            public const String MethodNeed866 = "このメソッドは秀丸エディタ v8.66 正式版以降で利用可能です。";
            public const String MethodNeed873 = "このメソッドは秀丸エディタ v8.73 正式版以降で利用可能です。";
            public static readonly String NoDllBindHandle866 = strDllFullPath + "をloaddllした際の束縛変数の値を特定できません";
        }

        // ★この関数ダミー用として必要。削除しないこと。
        public void _(String expression)
        {
        }

        // [EXPORT] debuginfo関数
        public void debuginfo(Object expression)
        {
            OutputDebugStream(expression.ToString());
        }

        // バージョン。hm.versionのため。読み取り専用
        static double _ver = 0;

        // [EXPORT version]
        public double version
        {
            get { return _ver; }
        }

        public static void InitVersion()
        {
            System.Diagnostics.FileVersionInfo vi = System.Diagnostics.FileVersionInfo.GetVersionInfo(strExecuteFullpath);
            _ver = 100 * vi.FileMajorPart + 10 * vi.FileMinorPart + 1 * vi.FileBuildPart + 0.01 * vi.FilePrivatePart;
        }

    }
}

