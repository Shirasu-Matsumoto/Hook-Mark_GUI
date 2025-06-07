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

        FLOAT dpi_x = 96.0f;
        FLOAT dpi_y = 96.0f;
        d2d1_factory->GetDesktopDpi(&dpi_x, &dpi_y);
        d2d1_render_target->SetDpi(dpi_x, dpi_y);

        d2d1_render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        
        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);

        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&d2d1_dwrite_factory)
        );
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            main_config.kifu_spacing * 0.8f,
            L"ja-JP",
            &text_format_default
        );
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            10.0f,  // 小さなフォントサイズ
            L"ja-JP",
            &text_format_label
        );

        return SUCCEEDED(hr);
    }

    void window_main::initialize(window_conf &config) {
        main_config = config;
        window_class.register_class();
        create_window();
        d2d1_initialize();
        grid_area_rectf = D2D1::RectF(
            main_config.margin,
            main_config.margin,
            main_config.grid_size_x - main_config.margin / 2,
            main_config.grid_and_kifu_size_y - main_config.margin / 2
        );
        kifu_area_rectf = D2D1::RectF(
            main_config.grid_size_x + main_config.margin / 2,
            main_config.margin,
            main_config.grid_size_x + main_config.kifu_size_x,
            main_config.grid_and_kifu_size_y - main_config.margin / 2
        );
        grid_area_rect = {
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.grid_size_x - main_config.margin / 2),
            static_cast<long>(main_config.grid_and_kifu_size_y - main_config.margin / 2)
        };
        kifu_area_rect = {
            static_cast<long>(main_config.grid_size_x + main_config.margin / 2),
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.grid_size_x + main_config.kifu_size_x),
            static_cast<long>(main_config.grid_and_kifu_size_y - main_config.margin / 2)
        };
    }

    void window_main::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hookmark_GUI_Main",
            L"Hookmark GUI",
            WS_OVERLAPPEDWINDOW,
            static_cast<int>(main_config.window_pos_x), static_cast<int>(main_config.window_pos_y),
            static_cast<int>(main_config.window_size_x), static_cast<int>(main_config.window_size_y),
            nullptr, nullptr, GetModuleHandle(nullptr), this
        );
        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ShowWindow(handle_window, SW_SHOW);
    }

    void window_main::show_file_load_dialog(std::wstring &path) {
        OPENFILENAMEW ofn;
        wchar_t szFile[MAX_PATH] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = handle_window;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = L"Hook-Mark棋譜ファイル (*.hmk)\0*.hmk\0すべてのファイル (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameW(&ofn)) {
            path = ofn.lpstrFile;
        }
        return;
    }

    void window_main::show_file_save_dialog(std::wstring &path) {
        OPENFILENAMEW ofn;
        wchar_t szFile[MAX_PATH] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = handle_window;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = L"Hook-Mark棋譜ファイル (*.hmk)\0*.hmk\0すべてのファイル (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"hmk";

        if (GetSaveFileNameW(&ofn)) {
            path = ofn.lpstrFile;
        }
    }

    void window_main::handle_exit() {
        PostQuitMessage(0);
    }

    void window_main::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));
        this->draw_grid();
        this->draw_kifu();
    }

    void window_main::draw_grid() {
        if (!d2d1_render_target || !d2d1_brush) return;
        
        const float grid_spacing = main_config.grid_spacing;

        d2d1_render_target->DrawLine(
            D2D1::Point2F(grid_area_rectf.left, grid_area_rectf.top),
            D2D1::Point2F(grid_area_rectf.left, grid_area_rectf.bottom),
            d2d1_brush,
            3.0f
        );
        d2d1_render_target->DrawLine(
            D2D1::Point2F(grid_area_rectf.right, grid_area_rectf.top),
            D2D1::Point2F(grid_area_rectf.right, grid_area_rectf.bottom),
            d2d1_brush,
            3.0f
        );
        d2d1_render_target->DrawLine(
            D2D1::Point2F(grid_area_rectf.left, grid_area_rectf.top),
            D2D1::Point2F(grid_area_rectf.right, grid_area_rectf.top),
            d2d1_brush,
            3.0f
        );
        d2d1_render_target->DrawLine(
            D2D1::Point2F(grid_area_rectf.left, grid_area_rectf.bottom),
            D2D1::Point2F(grid_area_rectf.right, grid_area_rectf.bottom),
            d2d1_brush,
            3.0f
        );

        float offset_x_mod = fmodf(grid_scroll_offset.x, grid_spacing);
        if (offset_x_mod < 0) offset_x_mod += grid_spacing;
        float offset_y_mod = fmodf(grid_scroll_offset.y, grid_spacing);
        if (offset_y_mod < 0) offset_y_mod += grid_spacing;
        float start_x = grid_area_rectf.left + grid_spacing - offset_x_mod;
        float start_y = grid_area_rectf.top + grid_spacing - offset_y_mod;

        for (float x = start_x; x < grid_area_rectf.right; x += grid_spacing) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(x, grid_area_rectf.top),
                D2D1::Point2F(x, grid_area_rectf.bottom),
                d2d1_brush,
                1.0f
            );
        }
        for (float y = start_y; y < grid_area_rectf.bottom; y += grid_spacing) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(grid_area_rectf.left, y),
                D2D1::Point2F(grid_area_rectf.right, y),
                d2d1_brush,
                1.0f
            );
        }

        d2d1_render_target->PushAxisAlignedClip(
            grid_area_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        auto &has_piece = board.has_piece();
        auto &is_first = board.is_first();
        auto x_range = has_piece.index_range();
        for (int x = x_range.min; x <= x_range.max; ++x) {
            if (has_piece.need_resize(x)) continue;
            auto y_range = has_piece[x].index_range();
            for (int y = y_range.min; y <= y_range.max; ++y) {
                if (has_piece[x].need_resize(y)) continue;
                if (!has_piece[x][y]) continue;

                float cx = grid_area_rectf.left + (x + 0.5f) * grid_spacing - grid_scroll_offset.x;
                float cy = grid_area_rectf.bottom - (y + 0.5f) * grid_spacing - grid_scroll_offset.y;
                float r = grid_spacing * 0.4f;

                if (cx + r < grid_area_rectf.left || cx - r > grid_area_rectf.right ||
                    cy + r < grid_area_rectf.top  || cy - r > grid_area_rectf.bottom)
                    continue;

                if (is_first[x][y]) {
                    d2d1_render_target->DrawEllipse(
                        D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r),
                        d2d1_brush,
                        3.0f
                    );
                } else {
                    float offset = r * 0.7f;
                    d2d1_render_target->DrawLine(
                        D2D1::Point2F(cx - offset, cy - offset),
                        D2D1::Point2F(cx + offset, cy + offset),
                        d2d1_brush,
                        3.0f
                    );
                    d2d1_render_target->DrawLine(
                        D2D1::Point2F(cx - offset, cy + offset),
                        D2D1::Point2F(cx + offset, cy - offset),
                        d2d1_brush,
                        3.0f
                    );
                }
            }
        }

        float grid_width  = grid_area_rectf.right - grid_area_rectf.left;
        float grid_height = grid_area_rectf.bottom - grid_area_rectf.top;

        int visible_x_min = static_cast<int>(std::ceil((grid_scroll_offset.x) / grid_spacing - 0.5f)) - 1;
        int visible_x_max = static_cast<int>(std::floor((grid_scroll_offset.x + grid_width) / grid_spacing - 0.5f)) + 1;
        for (int x = visible_x_min; x <= visible_x_max; ++x) {
            float cx = grid_area_rectf.left + (x + 0.5f) * grid_spacing - grid_scroll_offset.x;
            D2D1_RECT_F layout = D2D1::RectF(
                cx - grid_spacing / 2,
                grid_area_rectf.bottom - grid_spacing / 2,
                cx + grid_spacing / 2,
                grid_area_rectf.bottom
            );
            std::wstring txt = std::to_wstring(x);
            d2d1_render_target->DrawText(
                txt.c_str(),
                static_cast<UINT32>(txt.size()),
                text_format_label,
                layout,
                d2d1_brush
            );
        }

        int visible_y_min = static_cast<int>(std::ceil(-0.5f - grid_scroll_offset.y / grid_spacing)) - 1;
        int visible_y_max = static_cast<int>(std::floor((grid_height - grid_scroll_offset.y) / grid_spacing - 0.5f)) + 1;
        for (int y = visible_y_min; y <= visible_y_max; ++y) {
            float cy = grid_area_rectf.bottom - (y + 0.5f) * grid_spacing - grid_scroll_offset.y;
            D2D1_RECT_F layout = D2D1::RectF(
                grid_area_rectf.left,
                cy - grid_spacing / 2,
                grid_area_rectf.left + grid_spacing / 2,
                cy + grid_spacing / 2
            );
            std::wstring txt = std::to_wstring(y);
            d2d1_render_target->DrawText(
                txt.c_str(),
                static_cast<UINT32>(txt.size()),
                text_format_label,
                layout,
                d2d1_brush
            );
        }

        d2d1_render_target->PopAxisAlignedClip();
    }

    void window_main::draw_kifu() {
        if (!d2d1_render_target || !d2d1_brush) return;

        float kifu_spacing = main_config.kifu_spacing;

        d2d1_render_target->DrawLine(
            D2D1::Point2F(kifu_area_rectf.left, kifu_area_rectf.top),
            D2D1::Point2F(kifu_area_rectf.left, kifu_area_rectf.bottom),
            d2d1_brush,
            3.0f
        );
        d2d1_render_target->DrawLine(
            D2D1::Point2F(kifu_area_rectf.right, kifu_area_rectf.top),
            D2D1::Point2F(kifu_area_rectf.right, kifu_area_rectf.bottom),
            d2d1_brush,
            3.0f
        );
        d2d1_render_target->DrawLine(
            D2D1::Point2F(kifu_area_rectf.left, kifu_area_rectf.top),
            D2D1::Point2F(kifu_area_rectf.right, kifu_area_rectf.top),
            d2d1_brush,
            3.0f
        );
        d2d1_render_target->DrawLine(
            D2D1::Point2F(kifu_area_rectf.left, kifu_area_rectf.bottom),
            D2D1::Point2F(kifu_area_rectf.right, kifu_area_rectf.bottom),
            d2d1_brush,
            3.0f
        );

        d2d1_render_target->PushAxisAlignedClip(
            kifu_area_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        const auto &moves = current_kifu.data();
        for (unsigned int i = 0; i < moves.size(); i++) {
            std::wstring move_text = std::to_wstring(i + 1) + L": (" + std::to_wstring(moves[i].x) + L", " + std::to_wstring(moves[i].y) + L")";

            float y = kifu_area_rectf.top + i * main_config.kifu_spacing - kifu_scroll_offset.y;

            if (y + main_config.kifu_spacing < kifu_area_rectf.top || y > kifu_area_rectf.bottom) continue;

            D2D1_RECT_F layout_rect = D2D1::RectF(
                kifu_area_rectf.left + 10,
                y,
                kifu_area_rectf.right,
                y + main_config.kifu_spacing
            );

            d2d1_render_target->DrawText(
                move_text.c_str(),
                static_cast<UINT32>(move_text.size()),
                text_format_default,
                layout_rect,
                d2d1_brush
            );
        }

        d2d1_render_target->PopAxisAlignedClip();
    }

    void window_main::grid_scroll(float dx, float dy) {
        grid_scroll_offset.x += dx;
        grid_scroll_offset.y += dy;
    }

    void window_main::set_grid_scroll(float x, float y) {
        grid_scroll_offset.x = x;
        grid_scroll_offset.y = y;
    }

    D2D1_POINT_2F window_main::get_grid_scroll() const {
        return grid_scroll_offset;
    }

    void window_main::kifu_scroll(float dx, float dy) {
        kifu_scroll_offset.x += dx;
        kifu_scroll_offset.y += dy;

        // --- スクロール制限を追加 ---
        const auto &moves = current_kifu.data();
        float total_height = static_cast<float>(moves.size()) * main_config.kifu_spacing;
        float view_height = kifu_area_rectf.bottom - kifu_area_rectf.top;

        // 下限（上端）
        if (kifu_scroll_offset.y < 0.0f) {
            kifu_scroll_offset.y = 0.0f;
        }
        // 上限（下端）
        if (total_height > view_height) {
            float max_offset = total_height - view_height;
            if (kifu_scroll_offset.y > max_offset) {
                kifu_scroll_offset.y = max_offset;
            }
        } else {
            kifu_scroll_offset.y = 0.0f;
        }
    }

    void window_main::set_kifu_scroll(float dx, float dy) {
        kifu_scroll_offset.x = dx;
        kifu_scroll_offset.y = dy;
    }

    D2D1_POINT_2F window_main::get_kifu_scroll() const {
        return kifu_scroll_offset;
    }
}
