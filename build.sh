#!/bin/bash

# Functions

function build() {
echo "building... $1"
ThirdParty/Premake/premake5 gmake2
make config="$1"
compileFilesR
}

function compileFilesR() {
    echo "compiling shaders..."
    for f in $(find Source/Resources/Shaders -name '*.vert' -or -name '*.frag');
    do
        glslc "$f" -o Source/Resources/Shaders/Compiled/$( basename "$f" ).spv
    done
    echo "done"
}

function genworkspace() {
(
echo "{
    \"folders\": [
        {
            \"name\": \"Sad Engine\",
            \"path\": \".\"
     	},
    ],
    \"settings\": {}
}"
) > SadEngine.code-workspace
}


function gentasks() {
(
echo "{
    \"version\": \"2.0.0\",
    \"tasks\": ["

for c in "debug" "release"; do
for m in "editor" "game"; do
for p in "linux"; do

echo "        {
            \"label\": \"$c$m-x64_$p\",
            \"type\": \"shell\",
            \"command\": \"./build.sh\",
            \"args\": [
                \"$m\",
                \"$c\",
                \"$p\"
            ]"

if [ "$c$m$p" == "ReleaseGameLinux" ]; then echo "        }"
else echo "        },"; fi

done;done;done;

echo "    ]
}"
) > .vscode/tasks.json
}

function genccpppprops() {
    (
echo "{
    \"configurations\": ["

for c in "Debug" "Release"; do
for m in "Editor" "Game"; do
for p in "Linux"; do

echo "        {
            \"name\": \"$c$m-x64_$p\",
            \"includePath\": [
                \"\${workspaceFolder}/**\",
                \"$VULKAN_SDK_PATH\"
            ],
            \"defines\": [
                \"UNICODE\",
                \"_UNICODE\","

if [ "$c" == "Debug" ]; then echo "                \"DEBUG\","; fi
if [ "$m" == "Editor" ]; then echo "                \"SGE_WITH_EDITOR\","; fi

echo "                \"\"
            ],
            \"compilerPath\": \"$COMPILER_PATH\",
            \"cStandard\": \"c17\",
            \"cppStandard\": \"c++17\",
            \"intelliSenseMode\": \"gcc-x64\""
if [ "$c$m$p" == "ReleaseGameLinux" ]; then echo "        }"; else echo "        },"; fi

done;done;done;

echo     ],
echo     \"version\" : 4
echo }
) > .vscode/c_cpp_properties.json
}


function genlaunchs() {
    
(
echo "{
    \"version\": \"0.2.0\",
    \"configurations\": ["

for c in "Debug" "Release"; do
for m in "Editor" "Game"; do
for p in "Linux"; do

echo "        {
            \"name\": \"$c$m-x64_$p\",
            \"type\": \"cppdbg\",
            \"request\": \"launch\",
            \"program\": \"\${workspaceFolder}/Bin/$c$m-x64/SadEngine\",
            \"args\": [],
            \"stopAtEntry\": false,
            \"cwd\": \"\${workspaceFolder}/Source\",
            \"environment\": [],
            \"externalConsole\": true,
            \"MIMode\": \"gdb\",
            \"setupCommands\": [
                {
                    \"description\": \"Automatische Strukturierung und Einrückung für \\\"gdb\\\" aktivieren\",
                    \"text\": \"-enable-pretty-printing\",
                    \"ignoreFailures\": true
                }
            ]"
if [ "$c$m$p" == "ReleaseGameLinux" ]; then echo "        }"
else echo "        },"; fi

done;done;done;

echo "    ]
}"
) > .vscode/launch.json
}


# Program

COMPILER_PATH="/usr/bin/cpp"
VULKAN_SDK_PATH=""


if [ "$1" == "-help" ]
then
echo "
[modes]: editor, game
[configs]: debug, release
[platforms]: linux

[examples]
build:              ./build.sh editor debug linux
build and run:      ./build.sh editor debug linux -start
compile shaders:    ./build.sh -compileshaders
generate project:   ./build.sh -generateproject gmake2 (possible values: gmake2, codelite)
generate vscode:    ./build.sh -genvscode
"
elif [ "$1" == "-compileshaders" ]; then
compileFilesR

elif [ "$1" == "-generateproject" ]; then
ThirdParty/Premake/premake5 $2

elif [ "$1" == "-genvscode" ]; then
if !([ -e ".vscode" ]) ; then
mkdir ".vscode"
fi

genworkspace
gentasks
genccpppprops
genlaunchs

elif [ "$1" == "-codegen" ]; then
if [ "$2" == "release" ]; then
../../../Source/Tools/SGECodeGen/Bin/Release/SGECodeGen ./
elif [ "$2" == "debug" ]; then
../../../Source/Tools/SGECodeGen/Bin/Debug/SGECodeGen ./
fi
else

valid="true"
valid1="false"

for m in "editor" "game"
do
    if [ "$m" == "$1" ]
    then
        valid1="true"
    fi
done

valid=$valid1
valid1="false"

for m in "debug" "release"
do
    if [ "$m" == "$2" ]
    then
        valid1="true"
    fi
done

valid=$valid1
valid1="false"

for m in "linux"
do
    if [ "$m" == "$3" ]
    then
        valid1="true"
    fi
done

valid=$valid1


if [ "$valid" == "false" ]
then
    echo "
use: ./build.sh [mode] [config] [platform] or -help
"
else
    build $2$1-x64_$3

mode="none"
config="none"

    if [ "$4" == "-start" ]; then
    if [ "$1" == editor ]; then mode="Editor"
    elif [ "$1" == "game" ]; then mode="Game"
    fi
    if [ "$2" == "debug" ]; then config="Debug"
    elif [ "$2" == "release" ]; then config="Release"
    fi
    cd SadEngine
    ../Bin/$config$mode-x64/SadEngine
    fi
fi
fi