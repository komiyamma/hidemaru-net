// ★秀丸クラス
using System;
using System.Collections.Generic;

public sealed partial class hmRbDynamicLib
{
    public sealed partial class Hidemaru
    {

        public HmExplorerPane ExplorerPane = new HmExplorerPane();
        public class HmExplorerPane
        {
            public HmExplorerPane()
            {
                SetUnManagedDll();
            }

            // モードの設定
            public int SetMode(int mode)
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
            public int GetMode()
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
            public int LoadProject(string filepath)
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
            public int SaveProject(string filepath)
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
            public string GetProject()
            {
                try
                {
                    if (HmMacro._IsExecuting)
                    {
                        string cmd = @"dllfuncstr(loaddll(""HmExplorerPane""), ""GetProject"", hidemaruhandle(0))";
                        var Var = new HmMacro.TMacroVar();
                        string project_name = (string)Var[cmd];
                        if (String.IsNullOrEmpty(project_name))
                        {
                            return null;
                        }
                        return project_name;
                    }
                    else
                    {
                        string cmd = @"endmacro dllfuncstr(loaddll(""HmExplorerPane""), ""GetProject"", hidemaruhandle(0))";
                        var result = HmMacro._ExecEval(cmd);
                        string project_name = result.Message;
                        if (String.IsNullOrEmpty(project_name))
                        {
                            return null;
                        }
                        return result.Message;
                    }
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return null;
            }

            // GetCurrentDirする
            public string GetCurrentDir()
            {
                if (_ver < 885)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerNotFound + ":\n" + "GetCurrentDir");
                    return "";
                }
                try
                {
                    if (pExplorerPane_GetCurrentDir != null)
                    {
                        if (HmMacro._IsExecuting)
                        {
                            string cmd = @"dllfuncstr(loaddll(""HmExplorerPane""), ""GetCurrentDir"", hidemaruhandle(0))";
                            var Var = new HmMacro.TMacroVar();
                            string currentdir_name = (string)Var[cmd];
                            if (String.IsNullOrEmpty(currentdir_name))
                            {
                                return null;
                            }
                            return currentdir_name;
                        }
                        else
                        {
                            string cmd = @"endmacro dllfuncstr(loaddll(""HmExplorerPane""), ""GetCurrentDir"", hidemaruhandle(0))";
                            var result = HmMacro._ExecEval(cmd);
                            string currentdir_name = result.Message;
                            if (String.IsNullOrEmpty(currentdir_name))
                            {
                                return null;
                            }
                            return result.Message;
                        }
                    }
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedExplorerOperation + ":\n" + e.Message);
                }

                return null;
            }

            // GetUpdated
            public int GetUpdated()
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

            public IntPtr WindowHandle
            {
                get
                {
                    return pExplorerPane_GetWindowHandle(Hidemaru.WindowHandle);
                }
            }

            public IntPtr SendMessage(int commandID)
            {
                //
                // loaddll "HmExplorerPane.dll";
                // #h=dllfunc("GetWindowHandle",hidemaruhandle(0));
                // #ret=sendmessage(#h,0x111/*WM_COMMAND*/,251,0); //251=１つ上のフォルダ
                //
                return hmRbDynamicLib.SendMessage(pExplorerPane_GetWindowHandle(Hidemaru.WindowHandle), 0x111, commandID, IntPtr.Zero);
            }

        }
    }
}

