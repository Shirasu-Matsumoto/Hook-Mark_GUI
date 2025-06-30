#ifndef __HOOKMARK_GUI_WINDOW_BASE_HPP__
#define __HOOKMARK_GUI_WINDOW_BASE_HPP__

#if _WIN32_WINNT < 0x0A00
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0A00
#endif

#include <hookmark.hpp>
#include <hookmark_gui_helpers.hpp>
#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <versionhelpers.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <functional>

namespace hmgui {
    class window_base {
        public:
            HWND handle_window = nullptr;

            window_base() {}
            virtual ~window_base() {
                destroy_window();
            }
            virtual void initialize() { return; }
            virtual void create_window() { return; }
            virtual void show_window(int show_command = SW_SHOW) {
                if (handle_window) {
                    ShowWindow(handle_window, show_command);
                    UpdateWindow(handle_window);
                }
            }
            virtual void destroy_window() {
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
            WNDCLASSEXW window_class = {};

            wc_base() {}
            ~wc_base() {
                if (window_class.hInstance) {
                    UnregisterClassW(window_class.lpszClassName, window_class.hInstance);
                }
            }
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
            virtual void create_menu_item() {
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
            ~menu_item() {
                if (handle_menu) {
                    DestroyMenu(handle_menu);
                }
            }
            virtual void create_menu_item() {
                handle_menu = CreateMenu();
            }
            operator UINT_PTR() {
                return id;
            }
            operator UINT() {
                return static_cast<UINT>(id);
            }
    };

    class d2d1_button {
        public:
            ID2D1Factory *d2d1_factory = nullptr;
            IDWriteFactory *d2d1_dwrite_factory = nullptr;
            ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
            ID2D1SolidColorBrush *d2d1_brush = nullptr;
            IDWriteTextFormat *text_format = nullptr;
            std::wstring label_text;
            D2D1_RECT_F rectf = D2D1::RectF();
            RECT rect = { 0, 0, 0, 0 };
            int current_state = 0;
            bool enabled = true;

            d2d1_button() {}

            void initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, ID2D1HwndRenderTarget *i_d2d1_render_target,
                            ID2D1SolidColorBrush *i_d2d1_brush, IDWriteTextFormat *i_text_format, const std::wstring &i_label_text) {
                d2d1_factory = i_d2d1_factory;
                d2d1_dwrite_factory = i_d2d1_dwrite_factory;
                d2d1_render_target = i_d2d1_render_target;
                d2d1_brush = i_d2d1_brush;
                text_format = i_text_format;
                label_text = i_label_text;
                d2d1_factory->AddRef();
                d2d1_dwrite_factory->AddRef();
                d2d1_render_target->AddRef();
                d2d1_brush->AddRef();
                text_format->AddRef();
            }

            void redraw() {
                D2D1_COLOR_F color;
                if (enabled) {
                    switch (current_state) {
                        case 0: {
                            color = white_color;
                            break;
                        }
                        case 1: {
                            color = white_smoke_color;
                            break;
                        }
                        case 2: {
                            color = light_gray_color;
                            break;
                        }
                    }
                }
                else {
                    color = light_gray_color;
                }
                d2d1_brush->SetColor(color);
                d2d1_render_target->FillRectangle(rectf, d2d1_brush);
                d2d1_brush->SetColor(black_color);
                d2d1_render_target->DrawRectangle(rectf, d2d1_brush, 2.0f);
                d2d1_render_target->DrawText(
                    label_text.c_str(),
                    static_cast<UINT32>(label_text.size()),
                    text_format,
                    rectf,
                    d2d1_brush
                );
            }
    };
}

#endif
