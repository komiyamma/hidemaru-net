/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;
using System.IO.MemoryMappedFiles;
using System.Windows.Forms;

internal partial class HmPromptForm
{
    private static MemoryMappedFile m_Mmf = null;
    private static MemoryMappedViewAccessor m_Accessor = null;

    static void CreateSharedMemory()
    {
        if (m_Mmf == null)
        {
            // 共有メモリ生成
            m_Mmf = MemoryMappedFile.CreateOrOpen(@"HmPromptFormShared", sizeof(Int64));
            m_Accessor = m_Mmf.CreateViewAccessor();
        }
    }

    public static IntPtr ReadSharedMemory()
    {
        CreateSharedMemory();
        Int64 value = m_Accessor.ReadInt64(0);
        return new IntPtr(value);
    }

    public static void WriteSharedMemory(IntPtr hCurrentActiveWndHidemaru)
    {
        CreateSharedMemory();
        try
        {
            // テキストボックスの値を共有メモリに書き込む
            Int64 value = hCurrentActiveWndHidemaru.ToInt64();
            m_Accessor.Write(0, value);
        }
        catch (Exception e)
        {
            Trace.WriteLine(e.Message);
        }
    }
}