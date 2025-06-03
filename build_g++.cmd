@echo off
set "srcdir=%~dp0\src\"
set "outdir=%~dp0\build\"
g++.exe -o %outdir%\Hook-Mark_GUI.exe %srcdir%\hookmark_gui_main.cpp %srcdir%\hookmark_gui_window.cpp -O3 -march=native -mtune=native -static -lstdc++ -lgcc -ld2d1 -lgdi32 -mwindows -municode -I"%srcdir%\include" -I"%srcdir%" -std=c++23
