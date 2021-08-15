@echo off
cd %~dp0
echo %~dp0
SET CURRENT_TMP_DIR=%~dp0
SET CURRENT_TMP_DIR_WITHOUT_LAST_SEPARATOR=%CURRENT_TMP_DIR:~0,-1%
echo %~dp0

find hidemaru.exe "32bit edition"

if %ERRORLEVEL%==0 (
	echo "秀丸エディタ 32bit版";
    for %%A in (%~dp0*.dll) do (
        echo %%A
        C:\Windows\Microsoft.NET\Framework\v4.0.30319\ngen.exe install %%A /AppBase:"%CURRENT_TMP_DIR_WITHOUT_LAST_SEPARATOR%"
    )
)

find hidemaru.exe "64bit edition"

if %ERRORLEVEL%==0 (
	echo "秀丸エディタ 64bit版";
    for %%A in (%~dp0*.dll) do (
        echo %%A
        C:\Windows\Microsoft.NET\Framework64\v4.0.30319\ngen.exe install %%A /AppBase:"%CURRENT_TMP_DIR_WITHOUT_LAST_SEPARATOR%"
    )

)


