#include <hookmark_gui_window_base.hpp>

namespace hmgui {
    class wc_main final : public wc_base {
        public:
            wc_main() : wc_base() {}

            ATOM register_class() override {
                window_class = {
                    sizeof(WNDCLASSEXW),
                    CS_HREDRAW | CS_VREDRAW,
                    window_proc,
                    0, 0,
                    GetModuleHandle(nullptr),
                    LoadIconW(nullptr, IDI_APPLICATION),
                    LoadCursorW(nullptr, IDC_ARROW),
                    (HBRUSH)(COLOR_WINDOW + 1),
                    nullptr,
                    L"Hookmark_Main",
                    LoadIconW(nullptr, IDI_APPLICATION)
                };
                return RegisterClassExW(&window_class);
            }
    };

    class window_main final : public window_base {
        public:
            wc_main window_class;

            window_main() : window_base() {}

            void initialize() override {
                window_class.register_class();
                create_window();
            }

            void create_window() override {
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

            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override {
                switch (message) {
                    case WM_DESTROY: {
                        PostQuitMessage(0);
                        return 0;
                    }
                    case WM_PAINT: {
                        return 0;
                    }
                    default: {
                        return DefWindowProc(handle_window, message, w_param, l_param);
                    }
                }
            }
    };
}
