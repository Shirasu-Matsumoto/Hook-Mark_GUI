#ifndef __HOOKMARK_GUI_WINDOW_BASE_HPP__
#define __HOOKMARK_GUI_WINDOW_BASE_HPP__

#include <hookmark.hpp>
#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <versionhelpers.h>
#include <cmath>

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

    class menu_base {
        public:
            HMENU handle_menu;

            menu_base() : handle_menu(nullptr) {}
            virtual ~menu_base() {
                if (handle_menu) {
                    DestroyMenu(handle_menu);
                }
            }
            virtual void create_menu() {
                handle_menu = CreateMenu();
            }
            operator HMENU() {
                return handle_menu;
            }
    };

    class menu_item_popup {
        public:
            HMENU handle_menu;

            menu_item_popup() {
                create_menu_item();
            }
            ~menu_item_popup() {
                if (handle_menu) {
                    DestroyMenu(handle_menu);
                }
            }
            void create_menu_item() {
                handle_menu = CreatePopupMenu();
            }
            operator HMENU() {
                return handle_menu;
            }
            operator UINT_PTR() {
                return reinterpret_cast<UINT_PTR>(handle_menu);
            }
    };

    class menu_item {
        public:
            HMENU handle_menu;
            UINT_PTR id;

            menu_item() {
                create_menu_item();
            }
            menu_item(UINT_PTR _id) : id(_id) {
                create_menu_item();
            }
            void create_menu_item() {
                handle_menu = CreateMenu();
            }
            operator UINT_PTR() {
                return id;
            }
    };
}

#endif
