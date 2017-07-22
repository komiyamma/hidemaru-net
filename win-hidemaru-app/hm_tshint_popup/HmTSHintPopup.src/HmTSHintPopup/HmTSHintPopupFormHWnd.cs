using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

partial class HmTSHintPopupForm : Form
{
    [DllImport("user32.dll")]
    static extern IntPtr GetActiveWindow();

    [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern int GetClassName(IntPtr hWnd, StringBuilder lpClassName, int nMaxCount);

    [DllImport("user32.dll")]
    public static extern IntPtr GetForegroundWindow();

    [DllImport("user32.dll")]
    public static extern int GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

    private bool IsActiveWindowIsHidemaruMainWindow()
    {
        IntPtr hWnd = GetActiveWindow();
        StringBuilder ClassName = new StringBuilder(256);
        int nRet = GetClassName(hWnd, ClassName, ClassName.Capacity);
        if (ClassName.ToString().Contains("Hidemaru32Class"))
        {
            return true;
        }
        return false;
    }

    [DllImport("user32.dll")]
    static extern IntPtr WindowFromPoint(System.Drawing.Point p);

    [DllImport("user32.dll")]
    static extern bool GetCursorPos(out Point lpPoint);

    [DllImport("user32.dll", SetLastError = true)]
    static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

    [DllImport("kernel32.dll")]
    static extern uint GetCurrentProcessId();

    private bool IsUnderWindowIsCurrentProcessWindow()
    {

        Point po;
        if (GetCursorPos(out po))
        {
            IntPtr hWnd = WindowFromPoint(po);

            int processID1 = 0;
            int threadID = GetWindowThreadProcessId(hWnd, out processID1);

            uint processID2 = GetCurrentProcessId();

            if (processID1 == processID2)
            {
                return true;
            }
        }
        return false;
    }

}
