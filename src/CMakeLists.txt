cmake_minimum_required(VERSION 3.10)
project(Hook-Mark_GUI)
set(CMAKE_CXX_STANDARD 23)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(Hook-Mark_GUI WIN32
    hookmark_gui_main.cpp
    hookmark_gui_window.cpp
)

target_link_options(Hook-Mark_GUI PRIVATE "-mwindows")