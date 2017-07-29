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
    static void Main(string[] args)
    {
        const int STD_OUTPUT_HANDLE = -11;
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
}
