set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
"%VCVARS%"

set CXX="cl.exe"
set CC="cl.exe"

cmake --workflow --preset x64-all