﻿cmake_minimum_required(VERSION 3.10)
project(Hook-Mark_GUI)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(Hook-Mark_GUI WIN32
    hookmark_gui_main.cpp
    hookmark_gui_window.cpp
    hookmark_gui.rc
)

target_link_libraries(Hook-Mark_GUI
    d2d1.lib
    dwrite.lib
    dwmapi.lib
    ole32.lib
    uuid.lib
    gdi32.lib
    kernel32.lib
    comctl32.lib
    user32.lib
)
