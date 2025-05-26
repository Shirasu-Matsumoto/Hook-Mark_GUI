#ifndef __HOOKMARK_GUI_WINDOW_BASE_HPP__
#define __HOOKMARK_GUI_WINDOW_BASE_HPP__

#include <hookmark.hpp>
#include <windows.h>

namespace hmgui {
    class window_base {
        public:
            HWND handle_window;

            window_base() {}
            virtual ~window_base() {
                destroy_window();
            }
            virtual void initialize() { return; }
            virtual void create_window() { return; }
            void show_window(int show_command = SW_SHOW) {
                if (handle_window) {
                    ShowWindow(handle_window, show_command);
                    UpdateWindow(handle_window);
                }
            }
            void destroy_window() {
                if (handle_window) {
                    DestroyWindow(handle_window);
                    PostQuitMessage(0);
                    handle_window = nullptr;
                }
            }
            void call_function(void (*function)()) {
                if (function) {
                    function();
                }
            }
            virtual LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
                return DefWindowProc(handle_window, message, w_param, l_param);
            }
            operator HWND() {
                return handle_window;
            }
    };

    class wc_base {
        public:
            WNDCLASSEXW window_class;

            wc_base() {}
            static LRESULT CALLBACK window_proc(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
                window_base *this_window_ptr = reinterpret_cast<window_base *>(GetWindowLongPtr(handle_window, GWLP_USERDATA));
                if (this_window_ptr) {
                    return this_window_ptr->handle_message(handle_window, message, w_param, l_param);
                }
                return DefWindowProcW(handle_window, message, w_param, l_param);
            }
            virtual ATOM register_class() { return 0; }
            operator WNDCLASSEXW &() {
                return window_class;
            }
    };
}

#endif