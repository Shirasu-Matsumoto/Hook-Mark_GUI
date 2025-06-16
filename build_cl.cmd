@echo off

pushd %cd%
cd %~dp0\src

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

set "includes=/Iinclude /I."
set "libs=user32.lib gdi32.lib kernel32.lib comctl32.lib d2d1.lib dwrite.lib dwmapi.lib ole32.lib uuid.lib"
set "flags=/std:c++20 /O2 /EHsc /DUNICODE /D_UNICODE /link /SUBSYSTEM:WINDOWS /out:..\build\Hook-Mark_GUI.exe"

cl.exe hookmark_gui_main.cpp hookmark_gui_window.cpp %includes% %flags% %libs%

popd

exit /b
