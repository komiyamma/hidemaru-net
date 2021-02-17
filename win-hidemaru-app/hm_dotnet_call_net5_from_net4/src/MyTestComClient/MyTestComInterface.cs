using System;
using System.Runtime.InteropServices;

internal sealed class ContractGuids
{
    public const string ServerClass = "DB1797F5-7198-4411-8563-D05F4E904956";
    public const string ServerInterface = "BA9AC84B-C7FC-41CF-8B2F-1764EB773D4B";
}

[ComVisible(true)]
[Guid(ContractGuids.ServerInterface)]
[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
public interface IMyTestCOMServer
{
    /// <summary>
    /// 文字列を足す
    /// </summary>
    string AddString(string str1, string str2);
}



