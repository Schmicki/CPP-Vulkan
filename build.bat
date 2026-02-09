@echo off

cd %~dp0

set VULKAN_SDK_PATH=
set MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe
set COMPILER_PATH=C:/Program Files (x86)/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/cl.exe

if "%VULKAN_SDK_PATH%" == "" ( set VULKAN_SDK_PATH=%VULKAN_SDK:\=/%)

if "%1"=="-help" (
    call :printhelp2
    goto :end
) else if "%1"=="--help" (
    call :printhelp2
    goto :end
) else if "%1"=="-compileshaders" (
    echo compiling shaders...
    call :compileshaders
    goto :end
) else if "%1"=="-generateproject" (
    if "%2"=="" (
        "ThirdParty/Premake/premake5.exe" vs2026
    ) else (
        "ThirdParty/Premake/premake5.exe" %2
    )
    goto :end
) else if "%1"=="-genvscode" (

    if not exist .vscode ( mkdir .vscode )

    call :gentasks
    call :genworkspace
    call :genccppprops
    call :genlaunchs

    goto :end
) else if "%1"=="-codegen" (
    if "%2"=="release" (
        echo running code gen
        "Source/Tools/SGECodeGen/Bin/Release/SGECodeGen" Source/Engine/Types 
    ) else if "%2"=="debug" (
        echo running code gen
        "Source/Tools/SGECodeGen/Bin/Debug/SGECodeGen" Source/Engine/Types 
    )

    if "%3"=="" (
        "ThirdParty/Premake/premake5.exe" vs2026
    ) else (
        "ThirdParty/Premake/premake5.exe" %3
    )
    goto :end
)


set valid=false

for %%x in (Editor editor Game game) do (
    if "%%x"=="%1" (
        set valid=true
    )
)

if %valid%==false (
    call :printhelp
goto :end
)

set valid=false

for %%x in (Debug debug Release release) do (
    if "%%x"=="%2" (
        set valid=true
    )
)

if %valid%==false (
    call :printhelp
goto :end
)

set valid=false

for %%x in (Windows windows Linux linux) do (
    if "%%x"=="%3" (
        set valid=true
    )
)

if %valid%==false (
    call :printhelp
goto :end
)

"ThirdParty/Premake/premake5.exe" vs2026

if "%MSBUILD_PATH%"=="" (
if exist .build (
    set empty=0
) else (
    mkdir .build
)

if exist .build/msbuild_path.txt (
    set empty=0
) else (
    echo searching for msbuild...
    "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\bin\MSBuild.exe > .build/msbuild_path.txt
)
set /p MSBUILD_PATH= < .build/msbuild_path.txt
)

echo msuild path: %MSBUILD_PATH%

"%MSBUILD_PATH%" VulkanEngine.sln /property:Configuration=%2%1-x64 /property:Platform=%3

echo compiling shaders...
call :compileshaders
echo done

if "%4"=="-start" (
    cd VulkanEngine
    "../Bin/%2%1-x64/VulkanEngine.exe"
    cd ..
)

goto :end


:printhelp

echo use build [mode] [config] [platform]
echo or try '--help' form more information

exit /b


:printhelp2

echo.
echo [modes]: editor, game
echo [configs]: debug, release
echo [platforms]: windows
echo.
echo [examples]
echo build:              ./build editor debug windows
echo build and run:      ./build editor debug windows -start
echo compile shaders:    ./build -compileshaders
echo generate project:   ./build -generateproject (will default to vs2026)
echo generate project2:  ./build -generateproject vs2026 (possible values: vs2026, vs2017, codelite)
echo generate vscode:    ./build -genvscode

exit /b


:compileshaders
for /f %%f in ("Source/Resources/Shaders/*.vert") do (
    
    "%VULKAN_SDK_PATH%/bin/glslc.exe" %%~ff -o Source/Resources/Shaders/Compiled/%%~nxf.spv
)

for /f %%f in ("Source/Resources/Shaders/*.frag") do (

    "%VULKAN_SDK_PATH%/bin/glslc.exe" %%~ff -o Source/Resources/Shaders/Compiled/%%~nxf.spv
)
exit /b


:genworkspace

(
echo {
echo     "folders": [
echo         {
echo             "name": "Vulkan Engine",
echo             "path": "."
echo     	 },
echo     ],
echo     "settings": {}
echo }
) > VulkanEngine.code-workspace

exit /b


:gentasks

(
echo {
echo     "version": "2.0.0",
echo     "tasks": [

for %%c in (Debug, Release) do ( for %%m in (Editor, Game) do ( for %%p in (Windows) do (

echo         {
echo             "label": "%%c%%m-x64_%%p",
echo             "type": "shell",
echo             "command": "./build.bat",
echo             "args": ["%%m", "%%c", "%%p"],
echo             "group": "build"

if %%c%%m%%p == ReleaseGameWindows (
    echo         }
) else (
    echo         },
)

)))

echo     ]
echo }
) > .vscode/tasks.json
exit /b


:genccppprops

(
echo {
echo     "configurations": [

for %%c in (Debug, Release) do ( for %%m in (Editor, Game) do ( for %%p in (Windows) do (

echo         {
echo             "name": "%%c%%m-x64_%%p",
echo             "includePath": [
echo                 "${workspaceFolder}/**",
echo                 "%VULKAN_SDK_PATH%/Include"
echo             ],
echo             "defines": [
echo                 "UNICODE",
echo                 "_UNICODE",

if %%c == Debug ( echo                 "DEBUG", )
if %%m == Editor ( echo                 "SGE_WITH_EDITOR", )

echo                 ""
echo             ],
echo         "compilerPath": "%COMPILER_PATH%",
echo         "cStandard": "c17",
echo         "cppStandard": "c++17",
echo         "intelliSenseMode": "msvc-x64"
if %%c%%m%%p == ReleaseGameWindows ( echo         } ) else ( echo         }, )

)))

echo     ],
echo     "version" : 4
echo }
) > .vscode/c_cpp_properties.json

exit /b


:genlaunchs

(
echo {
echo     "version": "0.2.0",
echo     "configurations": [

for %%c in (Debug, Release) do ( for %%m in (Editor, Game) do ( for %%p in (Windows) do (

echo         {
echo             "name": "%%c%%m-x64_%%p",
echo             "type": "cppvsdbg",
echo             "request": "launch",
echo             "program": "${workspaceFolder}/Bin/%%c%%m-x64/VulkanEngine.exe",
echo             "args": [],
echo             "stopAtEntry": false,
echo             "cwd": "${workspaceFolder}/Source",
echo             "environment": [],
echo             "console": "externalTerminal"

if %%c%%m%%p == ReleaseGameWindows ( echo         } ) else ( echo         }, )

)))

echo     ]
echo }
) > .vscode/launch.json

exit /b


:end