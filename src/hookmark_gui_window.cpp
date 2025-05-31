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
            L"Hookmark_GUI_Main",
            LoadIconW(nullptr, MAKEINTRESOURCEW(32512))
        };
        return RegisterClassExW(&window_class);
    }

    bool window_main::d2d1_initialize() {
        if (d2d1_factory) return true;

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1_factory);
        if (FAILED(hr)) return false;

        RECT rect;
        GetClientRect(handle_window, &rect);

        D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES handle_window_props = D2D1::HwndRenderTargetProperties(handle_window, size);

        hr = d2d1_factory->CreateHwndRenderTarget(props, handle_window_props, &d2d1_render_target);
        if (FAILED(hr)) return false;

        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);
        return SUCCEEDED(hr);
    }

    void window_main::initialize() {
        window_class.register_class();
        create_window();
        d2d1_initialize();
    }

    void window_main::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hookmark_GUI_Main",
            L"Hookmark GUI",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr, nullptr, GetModuleHandle(nullptr), this
        );
        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ShowWindow(handle_window, SW_SHOW);
    }

    void window_main::draw_grid() {
        D2D1_POINT_2F scroll_offset = D2D1::Point2F(0.0f, 0.0f);

        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));

        for (float x = grid_area_rectf.left; x <= grid_area_rectf.right; x += 15.0f) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(x - scroll_offset.x, grid_area_rectf.top - scroll_offset.y),
                D2D1::Point2F(x - scroll_offset.x, grid_area_rectf.bottom - scroll_offset.y),
                d2d1_brush
            );
        }

        for (float y = grid_area_rectf.top; y <= grid_area_rectf.bottom; y += 15.0f) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(grid_area_rectf.left - scroll_offset.x, y - scroll_offset.y),
                D2D1::Point2F(grid_area_rectf.right - scroll_offset.x, y - scroll_offset.y),
                d2d1_brush
            );
        }
    }

    void window_main::grid_scroll(float dx, float dy) {
        scroll_offset.x += dx;
        scroll_offset.y += dy;
    }

    void window_main::set_grid_scroll(float x, float y) {
        scroll_offset.x = x;
        scroll_offset.y = y;
    }

    D2D1_POINT_2F window_main::get_grid_scroll() const {
        return scroll_offset;
    }
}
