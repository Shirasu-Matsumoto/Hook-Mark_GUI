@echo off

pushd %cd%
cd %~dp0\src

g++.exe -o ..\build\Hook-Mark_GUI.exe hookmark_gui_main.cpp hookmark_gui_window.cpp -I"./include" -static -lstdc++ -lgcc -lwinpthread -ldwmapi -ldwrite -ld2d1 -lkernel32 -luser32 -lgdi32 -lcomctl32 -luuid -lole32 -mwindows -municode

popd

exit /b
