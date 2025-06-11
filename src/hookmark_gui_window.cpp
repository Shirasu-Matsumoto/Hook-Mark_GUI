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

    bool window_main::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        d2d1_factory = i_d2d1_factory;
        d2d1_factory->AddRef();
        d2d1_dwrite_factory = i_d2d1_dwrite_factory;
        d2d1_dwrite_factory->AddRef();

        RECT rect;
        GetClientRect(handle_window, &rect);

        D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();

        D2D1_HWND_RENDER_TARGET_PROPERTIES handle_window_props = D2D1::HwndRenderTargetProperties(handle_window, size);

        HRESULT hr = d2d1_factory->CreateHwndRenderTarget(props, handle_window_props, &d2d1_render_target);
        if (FAILED(hr)) return false;

        UINT dpi = 96;
        if (IsWindows10OrGreater()) {
            dpi = GetDpiForWindow(handle_window);
        }

        FLOAT dpi_x = static_cast<FLOAT>(dpi);
        FLOAT dpi_y = static_cast<FLOAT>(dpi);
        d2d1_render_target->SetDpi(dpi_x, dpi_y);

        d2d1_render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        d2d1_render_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);

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
            10.0f,
            L"ja-JP",
            &text_format_label
        );

        for (int i = 1; i < 6; i++) {
            add_label_size(i);
        }

        return SUCCEEDED(hr);
    }

    void window_main::add_label_size(int i) {
        if (label_width.size() <= i) {
            label_width.resize(i + 1, 0.0f);
            label_height.resize(i + 1, 0.0f);
        }
        else if (label_width[i] != 0.0f) {
            return;
        }

        std::wstring txt(i, L'0');
        Microsoft::WRL::ComPtr<IDWriteTextLayout> temp_layout;

        HRESULT hr = d2d1_dwrite_factory->CreateTextLayout(
            txt.c_str(),
            static_cast<UINT32>(txt.size()),
            text_format_label,
            1000.0f,
            1000.0f,
            temp_layout.GetAddressOf()
        );

        float text_width = 0.0f, text_height = 0.0f;
        if (SUCCEEDED(hr)) {
            DWRITE_TEXT_METRICS metrics;
            temp_layout->GetMetrics(&metrics);
            text_width = metrics.width;
            text_height = metrics.height;
        }

        label_width[i] = text_width;
        label_height[i] = text_height;
    }

    void window_main::initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        main_config = config;
        grid_scroll_offset = D2D1::Point2F(-(main_config.grid_size_x - main_config.margin * 2) / 2 + main_config.grid_spacing / 2, (main_config.grid_and_kifu_size_y - main_config.margin * 2) / 2 - main_config.grid_spacing / 2);
        window_class.register_class();
        create_window();
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
        grid_area_rectf = D2D1::RectF(
            main_config.margin,
            main_config.margin,
            main_config.grid_size_x - main_config.margin,
            main_config.grid_and_kifu_size_y - main_config.margin
        );
        kifu_area_rectf = D2D1::RectF(
            main_config.grid_size_x + main_config.margin,
            main_config.margin,
            main_config.grid_size_x + main_config.kifu_size_x - main_config.margin,
            main_config.grid_and_kifu_size_y - main_config.margin
        );
        config_area_rectf = D2D1::RectF(
            main_config.grid_size_x + main_config.kifu_size_x + main_config.margin,
            main_config.margin,
            main_config.margin,
            main_config.grid_and_kifu_size_y - main_config.margin
        );
        GetWindowRect(handle_window, &window_area_rect);
        grid_area_rect = {
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.grid_size_x - main_config.margin),
            static_cast<long>(main_config.grid_and_kifu_size_y - main_config.margin)
        };
        kifu_area_rect = {
            static_cast<long>(main_config.grid_size_x + main_config.margin),
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.grid_size_x + main_config.kifu_size_x - main_config.margin),
            static_cast<long>(main_config.grid_and_kifu_size_y - main_config.margin)
        };
        config_area_rect = {
            static_cast<long>(main_config.grid_size_x + main_config.kifu_size_x + main_config.margin),
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.margin),
            static_cast<long>(main_config.grid_and_kifu_size_y - main_config.margin)
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
    }

    void window_main::show_file_load_dialog(std::wstring &path) {
        OPENFILENAMEW ofn;
        wchar_t szFile[MAX_PATH] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = handle_window;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = L"Hook-Mark棋譜ファイル (*.hmk, *.hmkif)\0*.hmk;*.hmkif\0すべてのファイル (*.*)\0*.*\0";
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
        ofn.lpstrFilter = L"Hook-Mark棋譜ファイル (*.hmk, *.hmkif)\0*.hmk;*.hmkif\0すべてのファイル (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"hmk";

        if (GetSaveFileNameW(&ofn)) {
            path = ofn.lpstrFile;
        }
    }

    void window_main::handle_exit() {
        PostQuitMessage(0);
        d2d1_render_target->Release();
        d2d1_brush->Release();
        d2d1_dwrite_factory->Release();
        d2d1_factory->Release();
    }

    void window_main::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));
        this->draw_grid();
        this->draw_kifu();
        this->draw_config();
    }

    void window_main::draw_grid() {
        if (!d2d1_render_target || !d2d1_brush) return;

        const float grid_spacing = main_config.grid_spacing;

        d2d1_render_target->DrawRectangle(
            grid_area_rectf,
            d2d1_brush,
            2.0f
        );

        float offset_x_mod = fmodf(grid_scroll_offset.x, grid_spacing);
        if (offset_x_mod < 0) offset_x_mod += grid_spacing;
        float offset_y_mod = fmodf(grid_scroll_offset.y, grid_spacing);
        if (offset_y_mod < 0) offset_y_mod += grid_spacing;
        float start_x = grid_area_rectf.left + grid_spacing - offset_x_mod;
        float start_y = grid_area_rectf.bottom - offset_y_mod;

        for (float x = start_x; x < grid_area_rectf.right; x += grid_spacing) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(x, grid_area_rectf.top),
                D2D1::Point2F(x, grid_area_rectf.bottom),
                d2d1_brush,
                1.0f
            );
        }
        for (float y = start_y; y >= grid_area_rectf.top; y -= grid_spacing) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(grid_area_rectf.left, y),
                D2D1::Point2F(grid_area_rectf.right, y),
                d2d1_brush,
                1.0f
            );
        }

        float axis_x = grid_area_rectf.left + (0) * grid_spacing - grid_scroll_offset.x;
        if (axis_x >= grid_area_rectf.left && axis_x <= grid_area_rectf.right) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(axis_x, grid_area_rectf.top),
                D2D1::Point2F(axis_x, grid_area_rectf.bottom),
                d2d1_brush,
                2.0f
            );
        }

        float axis_y = grid_area_rectf.bottom - (0) * grid_spacing - grid_scroll_offset.y;
        if (axis_y >= grid_area_rectf.top && axis_y <= grid_area_rectf.bottom) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(grid_area_rectf.left, axis_y),
                D2D1::Point2F(grid_area_rectf.right, axis_y),
                d2d1_brush,
                2.0f
            );
        }

        float axis_x1 = grid_area_rectf.left + (1) * grid_spacing - grid_scroll_offset.x;
        if (axis_x1 >= grid_area_rectf.left && axis_x1 <= grid_area_rectf.right) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(axis_x1, grid_area_rectf.top),
                D2D1::Point2F(axis_x1, grid_area_rectf.bottom),
                d2d1_brush,
                2.0f
            );
        }

        float axis_y1 = grid_area_rectf.bottom - (1) * grid_spacing - grid_scroll_offset.y;
        if (axis_y1 >= grid_area_rectf.top && axis_y1 <= grid_area_rectf.bottom) {
            d2d1_render_target->DrawLine(
                D2D1::Point2F(grid_area_rectf.left, axis_y1),
                D2D1::Point2F(grid_area_rectf.right, axis_y1),
                d2d1_brush,
                2.0f
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
                        2.0f
                    );
                } else {
                    float offset = r * 0.7f;
                    d2d1_render_target->DrawLine(
                        D2D1::Point2F(cx - offset, cy - offset),
                        D2D1::Point2F(cx + offset, cy + offset),
                        d2d1_brush,
                        2.0f
                    );
                    d2d1_render_target->DrawLine(
                        D2D1::Point2F(cx - offset, cy + offset),
                        D2D1::Point2F(cx + offset, cy - offset),
                        d2d1_brush,
                        2.0f
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
            std::wstring txt = std::to_wstring(x);

            add_label_size(static_cast<int>(txt.size()));

            float new_left = cx - label_width[txt.size()] / 2;

            D2D1_RECT_F layout = D2D1::RectF(
                new_left,
                grid_area_rectf.bottom - 17,
                new_left + grid_spacing,
                grid_area_rectf.bottom
            );

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
        for (int y = visible_y_max; y >= visible_y_min; --y) {
            float cy = grid_area_rectf.bottom - (y + 0.5f) * grid_spacing - grid_scroll_offset.y;

            std::wstring txt = std::to_wstring(y);
            D2D1_RECT_F layout = D2D1::RectF(
                grid_area_rectf.left + 7,
                cy - label_height[txt.size()] / 2,
                grid_area_rectf.left + grid_spacing / 2,
                cy - label_height[txt.size()] / 2
            );

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

        d2d1_render_target->DrawRectangle(
            kifu_area_rectf,
            d2d1_brush,
            2.0f
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

    void window_main::draw_config() {
        if (!d2d1_render_target || !d2d1_brush) return;

        d2d1_render_target->PushAxisAlignedClip(
            config_area_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        d2d1_render_target->DrawRectangle(
            config_area_rectf,
            d2d1_brush,
            2.0f
        );

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

        const auto &moves = current_kifu.data();
        float total_height = static_cast<float>(moves.size()) * main_config.kifu_spacing;
        float view_height = kifu_area_rectf.bottom - kifu_area_rectf.top;

        if (kifu_scroll_offset.y < 0.0f) {
            kifu_scroll_offset.y = 0.0f;
        }

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

    wc_newgame::wc_newgame() : wc_base() {}

    ATOM wc_newgame::register_class() {
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
            L"Hookmark_GUI_NewGame",
            LoadIconW(nullptr, MAKEINTRESOURCEW(32512))
        };
        return RegisterClassExW(&window_class);
    }

    void window_newgame::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hookmark_GUI_NewGame",
            L"Hook-Mark GUI - New game",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            NULL, NULL, GetModuleHandle(nullptr), NULL
        );
        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void window_newgame::initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        newgame_config = config;
        window_class.register_class();
        create_window();
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
    }

    bool window_newgame::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        d2d1_factory = i_d2d1_factory;
        d2d1_factory->AddRef();
        d2d1_dwrite_factory = i_d2d1_dwrite_factory;
        d2d1_dwrite_factory->AddRef();

        RECT rect;
        GetClientRect(handle_window, &rect);

        D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();

        D2D1_HWND_RENDER_TARGET_PROPERTIES handle_window_props = D2D1::HwndRenderTargetProperties(handle_window, size);

        HRESULT hr = d2d1_factory->CreateHwndRenderTarget(props, handle_window_props, &d2d1_render_target);
        if (FAILED(hr)) return false;

        UINT dpi = 96;
        if (IsWindows10OrGreater()) {
            dpi = GetDpiForWindow(handle_window);
        }

        FLOAT dpi_x = static_cast<FLOAT>(dpi);
        FLOAT dpi_y = static_cast<FLOAT>(dpi);
        d2d1_render_target->SetDpi(dpi_x, dpi_y);

        d2d1_render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);

        if (FAILED(hr)) return false;

        return true;
    }

    void window_newgame::handle_exit() {
        show_window(SW_HIDE);
    }

    void window_newgame::release() {
        DestroyWindow(handle_window);
    }
}
