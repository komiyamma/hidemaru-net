/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using System;
using System.Runtime.InteropServices;



// ★秀丸クラス
public sealed partial class hmRbDynamicLib
{
    public sealed partial class Hidemaru
    {
        // 秀丸本体から出ている関数群
        delegate IntPtr TGetCurrentWindowHandle();
        delegate IntPtr TGetTotalTextUnicode();
        delegate IntPtr TGetLineTextUnicode(int nLineNo);
        delegate IntPtr TGetSelectedTextUnicode();
        delegate int TGetCursorPosUnicode(ref int pnLineNo, ref int pnColumn);

        /*
        [StructLayout(LayoutKind.Sequential)]
        struct POINT
        {
            public int x;
            public int y;
        }
        delegate int TGetCursorPosUnicodeFromMousePos(out POINT lpPoint, ref int pnLineNo, ref int pnColumn);
        */
        delegate int TGetCursorPosUnicodeFromMousePos(IntPtr lpPoint, ref int pnLineNo, ref int pnColumn);

        delegate int TEvalMacro([MarshalAs(UnmanagedType.LPWStr)] String pwsz);
        delegate int TCheckQueueStatus();
        delegate int TAnalyzeEncoding([MarshalAs(UnmanagedType.LPWStr)] String pwszFileName, IntPtr lParam1, IntPtr lParam2);
        delegate IntPtr TLoadFileUnicode([MarshalAs(UnmanagedType.LPWStr)] String pwszFileName, int nEncode, ref int pcwchOut, IntPtr lParam1, IntPtr lParam2);


        // 秀丸本体から出ている関数群
        static TGetCurrentWindowHandle pGetCurrentWindowHandle;
        static TGetTotalTextUnicode pGetTotalTextUnicode;
        static TGetLineTextUnicode pGetLineTextUnicode;
        static TGetSelectedTextUnicode pGetSelectedTextUnicode;
        static TGetCursorPosUnicodeFromMousePos pGetCursorPosUnicodeFromMousePos;
        static TGetCursorPosUnicode pGetCursorPosUnicode;
        static TEvalMacro pEvalMacro;
        static TCheckQueueStatus pCheckQueueStatus;
        static TAnalyzeEncoding pAnalyzeEncoding;
        static TLoadFileUnicode pLoadFileUnicode;

        // OutputPaneから出ている関数群
        delegate int TOutputPane_Output(IntPtr hHidemaruWindow, byte[] encode_data);
        delegate int TOutputPane_Push(IntPtr hHidemaruWindow);
        delegate int TOutputPane_Pop(IntPtr hHidemaruWindow);
        delegate IntPtr TOutputPane_GetWindowHandle(IntPtr hHidemaruWindow);
        delegate int TOutputPane_SetBaseDir(IntPtr hHidemaruWindow, byte[] encode_data);

        static TOutputPane_Output pOutputPane_Output;
        static TOutputPane_Push pOutputPane_Push;
        static TOutputPane_Pop pOutputPane_Pop;
        static TOutputPane_GetWindowHandle pOutputPane_GetWindowHandle;
        static TOutputPane_SetBaseDir pOutputPane_SetBaseDir;

        // ExplorerPaneから出ている関数群
        delegate int TExplorerPane_SetMode(IntPtr hHidemaruWindow, IntPtr mode);
        delegate int TExplorerPane_GetMode(IntPtr hHidemaruWindow);
        delegate int TExplorerPane_LoadProject(IntPtr hHidemaruWindow, byte[] encode_project_file_path);
        delegate int TExplorerPane_SaveProject(IntPtr hHidemaruWindow, byte[] encode_project_file_path);
        delegate IntPtr TExplorerPane_GetProject(IntPtr hHidemaruWindow);
        delegate IntPtr TExplorerPane_GetWindowHandle(IntPtr hHidemaruWindow);
        delegate int TExplorerPane_GetUpdated(IntPtr hHidemaruWindow);
        delegate IntPtr TExplorerPane_GetCurrentDir(IntPtr hHidemaruWindow);

        static TExplorerPane_SetMode pExplorerPane_SetMode;
        static TExplorerPane_GetMode pExplorerPane_GetMode;
        static TExplorerPane_LoadProject pExplorerPane_LoadProject;
        static TExplorerPane_SaveProject pExplorerPane_SaveProject;
        static TExplorerPane_GetProject pExplorerPane_GetProject;
        static TExplorerPane_GetWindowHandle pExplorerPane_GetWindowHandle;
        static TExplorerPane_GetUpdated pExplorerPane_GetUpdated;
        static TExplorerPane_GetCurrentDir pExplorerPane_GetCurrentDir;

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GlobalLock(IntPtr hMem);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern int GlobalUnlock(IntPtr hMem);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GlobalFree(IntPtr hMem);

        // 秀丸本体のexeを指すモジュールハンドル
        static UnManagedDll hmExeHandle;
        static UnManagedDll hmOutputPaneHandle;
        static UnManagedDll hmExplorerPaneHandle;

        static void SetUnManagedDll()
        {
            InitVersion();

            // ver 8.6.6以上が対象
            if (_ver >= 866)
            {
                // 初めての代入のみ
                if (hmExeHandle == null)
                {
                    hmExeHandle = new UnManagedDll(strExecuteFullpath);

                    pGetTotalTextUnicode = hmExeHandle.GetProcDelegate<TGetTotalTextUnicode>("Hidemaru_GetTotalTextUnicode");
                    pGetLineTextUnicode = hmExeHandle.GetProcDelegate<TGetLineTextUnicode>("Hidemaru_GetLineTextUnicode");
                    pGetSelectedTextUnicode = hmExeHandle.GetProcDelegate<TGetSelectedTextUnicode>("Hidemaru_GetSelectedTextUnicode");
                    pGetCursorPosUnicode = hmExeHandle.GetProcDelegate<TGetCursorPosUnicode>("Hidemaru_GetCursorPosUnicode");
                    pEvalMacro = hmExeHandle.GetProcDelegate<TEvalMacro>("Hidemaru_EvalMacro");
                    pCheckQueueStatus = hmExeHandle.GetProcDelegate<TCheckQueueStatus>("Hidemaru_CheckQueueStatus");

                    if (_ver >= 873)
                    {
                        pGetCursorPosUnicodeFromMousePos = hmExeHandle.GetProcDelegate<TGetCursorPosUnicodeFromMousePos>("Hidemaru_GetCursorPosUnicodeFromMousePos");
                        pGetCurrentWindowHandle = hmExeHandle.GetProcDelegate<TGetCurrentWindowHandle>("Hidemaru_GetCurrentWindowHandle");
                    }
                    if (_ver >= 890)
                    {
                        pAnalyzeEncoding = hmExeHandle.GetProcDelegate<TAnalyzeEncoding>("Hidemaru_AnalyzeEncoding");
                        pLoadFileUnicode = hmExeHandle.GetProcDelegate<TLoadFileUnicode>("Hidemaru_LoadFileUnicode");
                    }

                    try
                    {
                        string exedir = System.IO.Path.GetDirectoryName(strExecuteFullpath);
                        hmOutputPaneHandle = new UnManagedDll(exedir + @"\HmOutputPane.dll");
                        pOutputPane_Output = hmOutputPaneHandle.GetProcDelegate<TOutputPane_Output>("Output");
                        pOutputPane_Push = hmOutputPaneHandle.GetProcDelegate<TOutputPane_Push>("Push");
                        pOutputPane_Pop = hmOutputPaneHandle.GetProcDelegate<TOutputPane_Pop>("Pop");
                        pOutputPane_GetWindowHandle = hmOutputPaneHandle.GetProcDelegate<TOutputPane_GetWindowHandle>("GetWindowHandle");

                        if (_ver >= 877)
                        {
                            pOutputPane_SetBaseDir = hmOutputPaneHandle.GetProcDelegate<TOutputPane_SetBaseDir>("SetBaseDir");
                        }
                    }
                    catch (Exception e)
                    {
                        OutputDebugStream(ErrorMsg.MethodNeedOutputNotFound + ":\n" + e.Message);
                    }

                    try
                    {
                        string exedir = System.IO.Path.GetDirectoryName(strExecuteFullpath);
                        hmExplorerPaneHandle = new UnManagedDll(exedir + @"\HmExplorerPane.dll");
                        pExplorerPane_SetMode = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_SetMode>("SetMode");
                        pExplorerPane_GetMode = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_GetMode>("GetMode");
                        pExplorerPane_LoadProject = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_LoadProject>("LoadProject");
                        pExplorerPane_SaveProject = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_SaveProject>("SaveProject");
                        pExplorerPane_GetProject = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_GetProject>("GetProject");
                        pExplorerPane_GetUpdated = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_GetUpdated>("GetUpdated");
                        pExplorerPane_GetWindowHandle = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_GetWindowHandle>("GetWindowHandle");

                        if (_ver >= 885)
                        {
                            pExplorerPane_GetCurrentDir = hmExplorerPaneHandle.GetProcDelegate<TExplorerPane_GetCurrentDir>("GetCurrentDir");
                        }
                    }
                    catch (Exception e)
                    {
                        OutputDebugStream(ErrorMsg.MethodNeedExplorerNotFound + ":\n" + e.Message);
                    }

                }
            }
        }
    }
}

