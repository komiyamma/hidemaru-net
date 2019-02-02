#
# HmDivideWordAndRubyChar.ps1
#
[Windows.Globalization.JapanesePhoneticAnalyzer, Windows.Globalization, ContentType=WindowsRuntime]

function GetJpWordDivider
{
    param(
        [parameter(ValueFromPipeline=$true)]
        [ValidateLength(1,8000)]
        [string[]]
        $Text,
         
        [switch]
        $MonoRuby
    )
    process
    {
        foreach($t in $Text)
        {
            [Windows.Globalization.JapanesePhoneticAnalyzer, Windows.Globalization, ContentType=WindowsRuntime]::GetWords($t, $MonoRuby)
        }
    }
}

GetJpWordDivider $Args[0]
