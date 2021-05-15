// ★秀丸クラス
using System;
using System.Collections.Generic;

internal sealed partial class hmNETDynamicLib
{
    internal sealed partial class Hidemaru
    {
        public sealed class ExplorerPane
        {
            static ExplorerPane()
            {
                SetUnManagedDll();
            }


            // モードの設定
            public static int SetMode(int mode)
            {
                try
                {
                    int result = pExplorerPane_SetMode(Hidemaru.WindowHandle, (IntPtr)mode);
                    return result;
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return 0;
            }

            // モードの取得
            public static int GetMode()
            {
                try
                {
                    int result = pExplorerPane_GetMode(Hidemaru.WindowHandle);
                    return result;
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return 0;
            }

            // LoadProjectする
            public static int LoadProject(string filepath)
            {
                try
                {
                    byte[] encode_data = HmOriginalEncodeFunc.EncodeWStringToOriginalEncodeVector(filepath);
                    int result = pExplorerPane_LoadProject(Hidemaru.WindowHandle, encode_data);
                    return result;
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return 0;
            }
            
            // SaveProjectする
            public static int SaveProject(string filepath)
            {
                try
                {
                    byte[] encode_data = HmOriginalEncodeFunc.EncodeWStringToOriginalEncodeVector(filepath);
                    int result = pExplorerPane_SaveProject(Hidemaru.WindowHandle, encode_data);
                    return result;
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return 0;
            }

            // GetProjectする
            public static string GetProject()
            {
                try
                {
                    if (hmNETDynamicLib.Hidemaru.Macro.IsExecuting)
                    {
                        string cmd = @"dllfuncstr(loaddll(""HmExplorerPane""), ""GetProject"", hidemaruhandle(0))";
                        string project_name = (string)hmNETDynamicLib.Hidemaru.Macro.Var[cmd];
                        return project_name;
                    }
                    else
                    {
                        string cmd = @"endmacro dllfuncstr(loaddll(""HmExplorerPane""), ""GetProject"", hidemaruhandle(0))";
                        var result = hmNETDynamicLib.Hidemaru.Macro.ExecEval(cmd);
                        return result.Message;
                    }
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return "";
            }

            // GetCurrentDirする
            public static string GetCurrentDir()
            {
                if (version < 885)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerNotFound + ":\n" + "GetCurrentDir");
                    return "";
                }
                try
                {
                    if (hmNETDynamicLib.Hidemaru.pExplorerPane_GetCurrentDir != null)
                    {
                        if (hmNETDynamicLib.Hidemaru.Macro.IsExecuting)
                        {
                            string cmd = @"dllfuncstr(loaddll(""HmExplorerPane""), ""GetCurrentDir"", hidemaruhandle(0))";
                            string currentdir_name = (string)hmNETDynamicLib.Hidemaru.Macro.Var[cmd];
                            return currentdir_name;
                        }
                        else
                        {
                            string cmd = @"endmacro dllfuncstr(loaddll(""HmExplorerPane""), ""GetCurrentDir"", hidemaruhandle(0))";
                            var result = hmNETDynamicLib.Hidemaru.Macro.ExecEval(cmd);
                            return result.Message;
                        }
                    }
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return "";
            }

            // GetUpdated
            public static int GetUpdated()
            {
                try
                {
                    int result = pExplorerPane_GetUpdated(Hidemaru.WindowHandle);
                    return result;
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return 0;
            }

            public static IntPtr WindowHandle
            {
                get
                {
                    return pExplorerPane_GetWindowHandle(Hidemaru.WindowHandle);
                }
            }

            public static IntPtr SendMessage(int commandID)
            {
                //
                // loaddll "HmExplorerPane.dll";
                // #h=dllfunc("GetWindowHandle",hidemaruhandle(0));
                // #ret=sendmessage(#h,0x111/*WM_COMMAND*/,251,0); //251=１つ上のフォルダ
                //
                return hmNETDynamicLib.SendMessage(ExplorerPane.WindowHandle, 0x111, commandID, IntPtr.Zero);
            }

        }
    }
}

