param([string]$Python = 'python')
$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
& $Python prepare.py
if ($LASTEXITCODE -ne 0) { throw 'Preparation failed.' }
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'MSVC C++ tools were not found.' }
New-Item -ItemType Directory -Force build | Out-Null
$batch = @"
@echo off
call "$vs\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /utf-8 /EHsc /O2 /MT /W4 tests\runner.cpp /Fo:build\tests.obj /Fe:build\tests.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /utf-8 /EHsc /O2 /MT /W4 tests\files.cpp /Fo:build\files.obj /Fe:build\files.exe
if errorlevel 1 exit /b 1
for %%f in (standalone\*.cpp) do (
 cl /nologo /std:c++17 /utf-8 /EHsc /W4 /Zs "%%f"
 if errorlevel 1 exit /b 1
)
"@
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'build\test-compile.cmd'), $batch, [Text.Encoding]::Default)
& cmd.exe /d /c build\test-compile.cmd
if ($LASTEXITCODE -ne 0) { throw 'Compilation check failed.' }
& $Python tests\test.py build\tests.exe
if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
& .\build\files.exe
if ($LASTEXITCODE -ne 0) { throw 'File tests failed.' }
