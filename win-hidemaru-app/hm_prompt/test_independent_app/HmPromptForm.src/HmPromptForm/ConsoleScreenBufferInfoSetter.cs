/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Runtime.InteropServices;
using System.Text;

internal partial class HmPromptForm
{
    [DllImport("kernel32.dll")]
    static extern bool SetConsoleTextAttribute(
        IntPtr hConsoleOutput,
        ushort wAttributes);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern IntPtr GetStdHandle(
        int nStdHandle);

    [DllImport("kernel32.dll")]
    static extern bool GetConsoleScreenBufferInfo(
        IntPtr hConsoleOutput,
        out CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo);

    [StructLayout(LayoutKind.Sequential)]
    private struct COORD
    {
        public short X;
        public short Y;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct SMALL_RECT
    {
        public short Left;
        public short Top;
        public short Right;
        public short Bottom;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct CONSOLE_SCREEN_BUFFER_INFO
    {
        public COORD dwSize;
        public COORD dwCursorPosition;
        public ushort wAttributes;
        public SMALL_RECT srWindow;
        public COORD dwMaximumWindowSize;
    }

    [DllImport("Kernel32", SetLastError = true)]
    static extern bool ReadConsoleOutputCharacter(IntPtr hConsoleOutput,[Out] StringBuilder lpCharacter, uint nLength, COORD dwReadCoord,out uint lpNumberOfCharsRead);

    /* WinCon.h
    #define FOREGROUND_BLUE      0x0001 
    #define FOREGROUND_GREEN     0x0002 
    #define FOREGROUND_RED       0x0004 
    #define FOREGROUND_INTENSITY 0x0008 
    #define BACKGROUND_BLUE      0x0010 
    #define BACKGROUND_GREEN     0x0020 
    #define BACKGROUND_RED       0x0040 
    #define BACKGROUND_INTENSITY 0x0080 
    //*/

    /*
    static void SetConsoleColor(string[] args)
    {
        const int FOREGROUND_GREEN = 2;
        const int FOREGROUND_INTENSITY = 8;
        const int BACKGROUND_BLUE = 16;

        CONSOLE_SCREEN_BUFFER_INFO bufferInfo = new CONSOLE_SCREEN_BUFFER_INFO();
        IntPtr hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        GetConsoleScreenBufferInfo(hConsole, out bufferInfo);

        Console.WriteLine("Hello!");

        SetConsoleTextAttribute(
            hConsole,
            FOREGROUND_GREEN
            | FOREGROUND_INTENSITY
            | BACKGROUND_BLUE);

        Console.WriteLine("World!");

        SetConsoleTextAttribute(hConsole, bufferInfo.wAttributes);

        Console.WriteLine("Bye!");
    }
    */



    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    private unsafe struct CONSOLE_FONT_INFO_EX
    {
        internal uint cbSize;
        internal uint nFont;
        internal COORD dwFontSize;
        internal int FontFamily;
        internal int FontWeight;
        internal fixed char FaceName[LF_FACESIZE];
    }
    private const int STD_OUTPUT_HANDLE = -11;
    private const int TMPF_TRUETYPE = 4;
    private const int LF_FACESIZE = 32;
    private static IntPtr INVALID_HANDLE_VALUE = new IntPtr(-1);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool SetCurrentConsoleFontEx(
        IntPtr consoleOutput,
        bool maximumWindow,
        ref CONSOLE_FONT_INFO_EX consoleCurrentFontEx);


    [DllImport("kernel32.dll", SetLastError = true)]
    static extern int SetConsoleFont(
        IntPtr hOut,
        uint dwFontNum
        );
    public static void SetConsoleFont(string fontName = "Lucida Console")
    {
        unsafe
        {
            IntPtr hnd = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hnd != INVALID_HANDLE_VALUE)
            {
                CONSOLE_FONT_INFO_EX info = new CONSOLE_FONT_INFO_EX();
                info.cbSize = (uint)Marshal.SizeOf(info);

                // Set console font to Lucida Console.
                CONSOLE_FONT_INFO_EX newInfo = new CONSOLE_FONT_INFO_EX();
                newInfo.cbSize = (uint)Marshal.SizeOf(newInfo);
                newInfo.FontFamily = TMPF_TRUETYPE;
                IntPtr ptr = new IntPtr(newInfo.FaceName);
                Marshal.Copy(fontName.ToCharArray(), 0, ptr, fontName.Length);

                // Get some settings from current font.
                newInfo.dwFontSize = new COORD();
                newInfo.dwFontSize.X = info.dwFontSize.X;
                newInfo.dwFontSize.Y = info.dwFontSize.Y;
                newInfo.FontWeight = 700;
                SetCurrentConsoleFontEx(hnd, false, ref newInfo);
            }
        }
    }





}
