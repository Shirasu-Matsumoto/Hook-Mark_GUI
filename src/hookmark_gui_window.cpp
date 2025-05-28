#include <hookmark_gui_window.hpp>

namespace hmgui {
    wc_main::wc_main() : wc_base() {}

    ATOM wc_main::register_class() {
        window_class = {
            sizeof(WNDCLASSEXW),
            CS_HREDRAW | CS_VREDRAW,
            window_proc,
            0, 0,
            GetModuleHandle(nullptr),
            LoadIconW(nullptr, MAKEINTRESOURCEW(32512)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)),
            (HBRUSH)(COLOR_WINDOW + 1),
            nullptr,
            L"Hookmark_Main",
            LoadIconW(nullptr, MAKEINTRESOURCEW(32512))
        };
        return RegisterClassExW(&window_class);
    }

    window_main::window_main() : window_base() {}

    void window_main::initialize() {
        window_class.register_class();
        create_window();
    }

    void window_main::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hookmark_Main",
            L"Hookmark GUI",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr, nullptr, GetModuleHandle(nullptr), this
        );
        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ShowWindow(handle_window, SW_SHOW);
    }

    LRESULT CALLBACK window_main::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
            case WM_SYSKEYDOWN: {
                switch (w_param) {
                    case 'F': {
                        PostMessageW(window_main::handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE, NULL), NULL);
                        break;
                    }
                }
                return 0;
            }
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
            case WM_COMMAND: {
                switch (LOWORD(w_param)) {
                    case ID_MENU_FILE: {
                        // last insert
                    }
                }
            }
            case WM_PAINT: {
                return 0;
            }
            default: {
                return DefWindowProc(handle_window, message, w_param, l_param);
            }
        }
    }

    void menu_main::create_menu() {
        handle_menu = CreateMenu();
        if (handle_menu) {
            AppendMenuW(handle_menu, MF_STRING, 1, L"ファイル(F)");
            AppendMenuW(handle_menu, MF_STRING, 2, L"編集(E)");
        }
    }
}
