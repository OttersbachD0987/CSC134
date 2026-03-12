# Ars Timoris

## Description

Ars Timoris is a CLI roguelike RPG with basic modding support. All data files can be found in the Data folder, and end with the .data file extension. A custom interpreted scripting language is also used for this project.

## Building

In all operating systems you will require CMake, and due to how I have set it up, you must have CXX & CC as environment variables. Please do note, this assumes your working directory is the same as this README.md file, you should observe the following file structure:

```nohighlight
M3Lab1/
├─ ArsTimoris/
│  ├─ Assets/
│  ├─ include/
│  ├─ src/
│  └─ CMakeLists.txt
├─ vcpkg/
├─ .gitignore
├─ CMakeLists.txt
├─ CMakePresets.json
├─ README.md
└─ vcpkg.json
```

### Linux (Bash) - GCC/G++

```bash
export CXX=g++
export CC=gcc

cmake --workflow --preset x64-all
```

### Windows (Batch) - Clang

If you are using Visual Studio Build Tools then you must run the following to get access to clang:

```batch
set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
"%VCVARS%"
```

Then run the following to build it:

```batch
set CXX=cl
set CC=cl

cmake --workflow --preset x64-all
```

### Windows (Powershell) - Clang

```powershell
$env:VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
& $env:VCVARS
```

Then run the following to build it:

```powershell
$env:CXX='cl.exe'
$env:CC='cl.exe'

cmake --workflow --preset x64-all
```

***

This will build both a Debug build and a Release build, found in: `out/build/x64`.

- Debug: `out/build/x64/Debug/ArsTimoris.exe`
- Release: `out/build/x64/Release/ArsTimoris.exe`

## Running

### Linux (Bash)

```bash
(cd ./out/build/x64/Release; ./ArsTimoris)
```

### Windows (Command Prompt)

```batch
START "" /D ".\out\build\x64\Release" ".\out\build\x64\Release\ArsTimoris.exe"
```

### Windows (Powershell)

```powershell
Start-Process -FilePath ".\out\build\x64\Release\ArsTimoris.exe" -WorkingDirectory ".\out\build\x64\Release"
```
