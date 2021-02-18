using System;
using System.Runtime.InteropServices;

internal sealed class ContractGuids
{
    public const string ServerClass = "35453D63-CC4C-467F-BD2B-457C417B15A5";
    public const string ServerInterface = "473100CB-2110-4264-BCEB-8394C487D891";
}

[ComVisible(true)]
[Guid(ContractGuids.ServerInterface)]
[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
public interface IMyTestCOMServer
{
    /// <summary>
    /// 文字列を足す
    /// </summary>
    string AddString(string str1, string str2, string[] list);
}

