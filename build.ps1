param([string]$Python = 'python')
$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
& $Python prepare.py
if ($LASTEXITCODE -ne 0) { throw 'Source preparation failed.' }
$sdk = Join-Path $PSScriptRoot '.deps\webview2'
if (!(Test-Path "$sdk\build\native\include\WebView2.h")) {
    New-Item -ItemType Directory -Force .deps | Out-Null
    Invoke-WebRequest 'https://api.nuget.org/v3-flatcontainer/microsoft.web.webview2/1.0.3650.58/microsoft.web.webview2.1.0.3650.58.nupkg' -OutFile .deps\webview2.zip
    Expand-Archive .deps\webview2.zip $sdk -Force
}
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (!(Test-Path $vswhere)) { throw 'Install Visual Studio Build Tools with Desktop development with C++.' }
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'MSVC C++ tools were not found.' }
New-Item -ItemType Directory -Force build | Out-Null
$batch = @"
@echo off
call "$vs\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cd /d "$PSScriptRoot\src"
rc /nologo /fo "$PSScriptRoot\build\app.res" app.rc
if errorlevel 1 exit /b 1
cd /d "$PSScriptRoot"
cl /nologo /std:c++17 /utf-8 /EHsc /O2 /MT /W4 /DUNICODE /D_UNICODE /I"$sdk\build\native\include" src\main.cpp /Fo:build\main.obj /Fe:build\CyberLab.exe /link /SUBSYSTEM:WINDOWS /MANIFEST:NO build\app.res "$sdk\build\native\x64\WebView2LoaderStatic.lib" user32.lib ole32.lib oleaut32.lib shell32.lib shlwapi.lib comdlg32.lib gdi32.lib version.lib advapi32.lib
exit /b %errorlevel%
"@
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'build\compile.cmd'), $batch, [Text.Encoding]::Default)
& cmd.exe /d /c build\compile.cmd
if ($LASTEXITCODE -ne 0) { throw 'C++ build failed.' }
Write-Host 'Ready: build\CyberLab.exe'
