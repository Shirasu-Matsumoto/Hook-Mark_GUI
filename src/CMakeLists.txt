include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(Hook-Mark_GUI WIN32
    hookmark_gui_main.cpp
    hookmark_gui_window.cpp
)

target_link_options(Hook-Mark_GUI PRIVATE "-mwindows")