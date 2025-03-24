@echo off

set VARIANT=debug
rem set VARIANT=release

setlocal enabledelayedexpansion

chcp 65001>nul

if "%1" == "detect_git" (
    call :detect_git
) else if "%1" == "detect_llvm" (
    call :detect_llvm
) else if "%1" == "detect_msvc" (
    call :detect_msvc
) else (
    call :build %1 %VARIANT%
)

exit /b %errorlevel%

:detect_git
where /q git.exe
if ERRORLEVEL 1 (
    echo Error: git.exe not found.
    exit /B 1
) else (
    rem echo Found git.exe
    set GIT_EXE=git.exe
    exit /B 0
)

:detect_llvm
for /F "tokens=1,2*" %%i in ('reg query "HKLM\SOFTWARE\WOW6432Node\LLVM\LLVM" /v ""') DO (
	if "%%i"=="(Default)" (
        if not "%%k"=="" (
            set LLVM=%%k
            goto :detect_llvm_done
        )
	)
)
echo Error: LLVM not found
exit /B 1
:detect_llvm_done
rem echo Found %LLVM%
set CLANG="%LLVM%\bin\clang++.exe"
exit /B 0

:detect_msvc
set "VSWHERE_EXE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set WORKLOAD=Microsoft.VisualStudio.Workload.NativeDesktop
if exist "%VSWHERE_EXE%" (
    for /f "tokens=*" %%a in ('"%VSWHERE_EXE%" -utf8 -prerelease -latest -requires %WORKLOAD% -find **\vcvarsall.bat') do (
        set "VCVARSALL=%%a"
        goto :detect_msvc_done
    )
    echo Error: workload %WORKLOAD% not installed
    exit /B 1
) else (
    echo Error: vswhere.exe not found
    exit /B 1
)
:detect_msvc_done
rem echo Found %VCVARSALL%
exit /B 0

:build

cd /D "%~dp0"

if exist output\ (
    rem
) else (
    mkdir output
)


where /q cl.exe
if ERRORLEVEL 1 (
    rem echo cl.exe not found yet
    if exist output\env (
        rem echo Found output\env
    ) else (
        rem echo Creating output\env
        call :detect_msvc
        if ERRORLEVEL 1 (
            echo Error: vcvarsall.bat file not found
            exit /b !errorlevel!
        ) else (
            rem echo Found vcvarsall.bat
            echo "!VCVARSALL!"
            %comspec% /c "call "!VCVARSALL!" x64 && set > output\env"
        )
    )
    for /f "tokens=1,2 delims==" %%i in (output\env) do set "%%i=%%j"
) else (
    rem echo Found cl.exe
)

if exist cut\ (
  rem echo Found Cut! sources
) else (
    call :detect_git
    if ERRORLEVEL 1 (
        echo Error: git.exe is missing. Skipping Cut^^! installation.
        goto skip_cut
    ) else (
        git clone https://github.com/segfault-survivor/cut || exit /B 1
    )
)


if exist cut\cut-cl.exe (
    rem echo Found cut-cl.exe
) else (
    pushd cut
    echo Building Cut^^! Please wait.
    call build.bat
    copy /B /Y "cut-clang++.exe" cut-cl.exe
    popd
)

:skip_cut

if exist cut\cut-cl.exe (
    set COMPILER=cut\cut-cl.exe
) else (
    set COMPILER=cl.exe
    echo WARNING: cut-cl.exe not found, incremental compilation will be extremely slow
)

if "%2" == "release" (
    set "OPTIONS=/O2 /DNDEBUG"
    echo WARNING: OPTIMIZATION=ON. Compilation in Release mode is extremely slow
) else (
    set "OPTIONS=/Od"
    echo WARNING: OPTIMIZATION=OFF. Executable in Debug mode is extremely slow
)

rem ----------------------------------CUT HERE----------------------------------

%COMPILER%                         ^
    %OPTIONS%                      ^
    /Z7                            ^
    /nologo                        ^
    /I.                            ^
    /DNOMINMAX                     ^
    /DWIN32_LEAN_AND_MEAN_NO       ^
    /D_WIN32_WINNT=0x0601          ^
    /EHsc                          ^
    /permissive-                   ^
    /Zc:preprocessor               ^
    /Zc:__cplusplus                ^
    /std:c++latest                 ^
    /utf-8                         ^
    /fp:strict                     ^
    /c main-windows.cpp            ^
    /Fooutput/main.obj || exit /b 1

link /nologo        ^
    output/main.obj ^
    user32.lib      ^
    gdi32.lib       ^
    Msimg32.lib     ^
    Gdiplus.lib     ^
    Ole32.lib       ^
    /DEBUG:FULL     ^
    /OUT:output/main.exe || exit /b 1


if "%1" == "build_only" (
    rem echo not starting
    exit /b 0
) else (
    echo.

    output\main.exe    
    set RET=!ERRORLEVEL!

    echo.
    echo Exit code: !RET!
    exit /b !RET!
)
