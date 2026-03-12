# Ars Timoris

## Description

Ars Timoris is a CLI roguelike RPG with basic modding support. All data files can be found in the Data folder, and end with the .data file extension. A custom interpreted scripting language is also used for this project.

## Building

To build Ars Timoris use the following commands:

### Linux (GCC/G++)

```bash
export CXX=g++
export CC=gcc

cmake --workflow --preset x64-all
```

### Windows & MacOS (Clang)

If you are using Visual Studio Build Tools then you must run the following to get access to clang:

```powershell
set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
"%VCVARS%"
```

```powershell
set CXX=cl
set CC=cl

cmake --workflow --preset x64-all
```
