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
            L"Hook-Mark_GUI_Main",
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

        d2d1_render_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            config_ref.kifu_spacing * 0.8f,
            L"ja-JP",
            &text_format_kifu
        );
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            config_ref.label_size,
            L"ja-JP",
            &text_format_label
        );
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,
            L"ja-JP",
            &text_format_config
        );
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            20.0f,
            L"ja-JP",
            &text_format_game_control_button_label
        );
        if (FAILED(hr)) return false;

        text_format_game_control_button_label->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        text_format_game_control_button_label->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

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

    void window_main::update_rect() {
        GetClientRect(handle_window, &client_area_rect);
        client_area_rectf = rect_to_rectf(client_area_rect);
        BOOL is_dwm_enabled = FALSE;
        if (SUCCEEDED(DwmIsCompositionEnabled(&is_dwm_enabled)) && is_dwm_enabled) {
            DwmGetWindowAttribute(handle_window, DWMWA_EXTENDED_FRAME_BOUNDS, &window_area_rect, sizeof(RECT));
        } else {
            GetWindowRect(handle_window, &window_area_rect);
        }
        window_area_rectf = rect_to_rectf(window_area_rect);
        grid_area_rectf = D2D1::RectF(
            config_ref.margin,
            config_ref.margin,
            config_ref.grid_size_x - config_ref.margin,
            config_ref.grid_and_kifu_size_y - config_ref.margin
        );
        kifu_area_rectf = D2D1::RectF(
            config_ref.grid_size_x + config_ref.margin,
            config_ref.margin,
            config_ref.grid_size_x + config_ref.kifu_size_x - config_ref.margin,
            config_ref.grid_and_kifu_size_y - config_ref.margin
        );
        config_area_rectf = D2D1::RectF(
            config_ref.grid_size_x + config_ref.kifu_size_x + config_ref.margin,
            config_ref.margin,
            client_area_rectf.right - config_ref.margin,
            config_ref.grid_and_kifu_size_y - config_ref.margin
        );
        do_over_button_area_rectf = D2D1::RectF(
            config_ref.margin,
            config_ref.grid_and_kifu_size_y + config_ref.margin,
            100.0f - config_ref.margin,
            config_ref.grid_and_kifu_size_y + 50.0f - config_ref.margin
        );
        resign_button_area_rectf = D2D1::RectF(
            100.0f + config_ref.margin,
            config_ref.grid_and_kifu_size_y + config_ref.margin,
            200.0f - config_ref.margin,
            config_ref.grid_and_kifu_size_y + 50.0f - config_ref.margin
        );
        grid_area_rect = rectf_to_rect(grid_area_rectf);
        kifu_area_rect = rectf_to_rect(kifu_area_rectf);
        config_area_rect = rectf_to_rect(config_area_rectf);
        do_over_button_area_rect = rectf_to_rect(do_over_button_area_rectf);
        resign_button_area_rect = rectf_to_rect(resign_button_area_rectf);
        grid_area_clip_rectf = cliped_rectf(grid_area_rectf);
        kifu_area_clip_rectf = cliped_rectf(kifu_area_rectf);
        config_area_clip_rectf = cliped_rectf(config_area_rectf);
    }

    void window_main::update_config() {
        scroll_speed = config_ref.grid_spacing / 0.3f;
    }

    void window_main::initialize_scroll() {
        grid_scroll_offset = D2D1::Point2F(-(config_ref.grid_size_x - config_ref.margin * 2) / 2 + config_ref.grid_spacing / 2, (config_ref.grid_and_kifu_size_y - config_ref.margin * 2) / 2 - config_ref.grid_spacing / 2);
        kifu_scroll_offset = D2D1::Point2F();
        config_scroll_offset = D2D1::Point2F();
    }

    void window_main::initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        config_ref = config;
        initialize_scroll();
        window_class.register_class();
        create_window();
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
        update_rect();
    }

    void window_main::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_Main",
            L"Hook-Mark GUI",
            WS_OVERLAPPEDWINDOW,
            static_cast<int>(config_ref.window_pos_x), static_cast<int>(config_ref.window_pos_y),
            static_cast<int>(config_ref.window_size_x), static_cast<int>(config_ref.window_size_y),
            nullptr, nullptr, GetModuleHandle(nullptr), this
        );

        DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(handle_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void window_main::show_file_load_dialog(std::wstring &result) {
        std::wstring file_path;
        IFileOpenDialog *file_open_dialog = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
        if (SUCCEEDED(hr)) {
            COMDLG_FILTERSPEC rg_spec[] = {
                { L"Hook-Mark棋譜ファイル (*.hmk, *.hmkif)", L"*.hmk;*.hmkif" },
                { L"すべてのファイル (*.*)", L"*.*" }
            };
            hr = file_open_dialog->SetFileTypes(ARRAYSIZE(rg_spec), rg_spec);
            if (SUCCEEDED(hr)) {
                hr = file_open_dialog->SetFileTypeIndex(1);
            }

            hr = file_open_dialog->Show(nullptr);
            if (SUCCEEDED(hr)) {
                IShellItem *shell_item = nullptr;
                hr = file_open_dialog->GetResult(&shell_item);
                if (SUCCEEDED(hr)) {
                    PWSTR psz_file_path = nullptr;
                    hr = shell_item->GetDisplayName(SIGDN_FILESYSPATH, &psz_file_path);
                    if (SUCCEEDED(hr) && psz_file_path) {
                        file_path = psz_file_path;
                        CoTaskMemFree(psz_file_path);
                    }
                }
                if (shell_item) {
                    shell_item->Release();
                }
            }
            file_open_dialog->Release();
        }
        result = file_path;
    }

    void window_main::show_file_save_dialog(std::wstring &path) {
        IFileSaveDialog *file_save_dialog = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog,
                                    nullptr,
                                    CLSCTX_ALL,
                                    IID_PPV_ARGS(&file_save_dialog));
        if (SUCCEEDED(hr)) {
            COMDLG_FILTERSPEC rg_spec[] = {
                { L"Hook-Mark棋譜ファイル (*.hmk, *.hmkif)", L"*.hmk;*.hmkif" },
                { L"すべてのファイル (*.*)", L"*.*" }
            };
            hr = file_save_dialog->SetFileTypes(ARRAYSIZE(rg_spec), rg_spec);
            if (SUCCEEDED(hr)) {
                hr = file_save_dialog->SetFileTypeIndex(1);
            }

            file_save_dialog->SetDefaultExtension(L"hmk");

            hr = file_save_dialog->Show(nullptr);
            if (SUCCEEDED(hr)) {
                IShellItem* shell_item = nullptr;
                hr = file_save_dialog->GetResult(&shell_item);
                if (SUCCEEDED(hr)) {
                    PWSTR psz_file_path = nullptr;
                    hr = shell_item->GetDisplayName(SIGDN_FILESYSPATH, &psz_file_path);
                    if (SUCCEEDED(hr) && psz_file_path) {
                        path = psz_file_path;
                        CoTaskMemFree(psz_file_path);
                    }
                    shell_item->Release();
                }
            }
            file_save_dialog->Release();
        }
    }

    void window_main::handle_exit() {
        initialize_scroll();
        PostQuitMessage(0);
        d2d1_render_target->Release();
        d2d1_brush->Release();
        d2d1_dwrite_factory->Release();
        d2d1_factory->Release();
        text_format_kifu->Release();
        text_format_config->Release();
        text_format_label->Release();
    }

    void window_main::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));
        this->draw_grid();
        this->draw_kifu();
        this->draw_config();
        this->draw_game_control();
    }

    void window_main::draw_grid() {
        if (!d2d1_render_target || !d2d1_brush) return;

        const float grid_spacing = config_ref.grid_spacing;

        d2d1_render_target->DrawRectangle(
            grid_area_rectf,
            d2d1_brush,
            2.0f
        );

        float offset_x_mod = std::round(fmodf(grid_scroll_offset.x, grid_spacing));
        if (offset_x_mod < 0) offset_x_mod += grid_spacing;
        float offset_y_mod = std::round(fmodf(grid_scroll_offset.y, grid_spacing));
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
            grid_area_clip_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        draw_board();

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
                grid_area_rectf.left + grid_spacing * 10,
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

    void window_main::draw_board() {
        const float grid_spacing = config_ref.grid_spacing;

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
                    d2d1_brush->SetColor(red_color);
                    d2d1_render_target->DrawEllipse(
                        D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r),
                        d2d1_brush,
                        2.0f
                    );
                } else {
                    d2d1_brush->SetColor(blue_color);
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

        if (!current_kifu.empty()) {
            hm::pos p = current_kifu[kifu_current_turn];
            d2d1_brush->SetColor(yellow_color);
            d2d1_render_target->DrawRectangle(
                D2D1::RectF(
                    grid_area_rectf.left + (p.x + 1.0f) * grid_spacing - grid_scroll_offset.x,
                    grid_area_rectf.bottom - (p.y + 1.0f) * grid_spacing - grid_scroll_offset.y,
                    grid_area_rectf.left + (p.x) * grid_spacing - grid_scroll_offset.x,
                    grid_area_rectf.bottom - (p.y) * grid_spacing - grid_scroll_offset.y
                ),
                d2d1_brush,
                3.0f
            );
        }

        d2d1_brush->SetColor(black_color);
    }

    void window_main::draw_kifu_single(const hm::pos &move, unsigned int turn) {
        float y = kifu_area_rectf.top + turn * config_ref.kifu_spacing - kifu_scroll_offset.y;

        if (y + config_ref.kifu_spacing < kifu_area_rectf.top || y > kifu_area_rectf.bottom) return;

        D2D1_RECT_F single_kifu_rectf = D2D1::RectF(
            kifu_area_rectf.left,
            y,
            kifu_area_rectf.right,
            y + config_ref.kifu_spacing
        );

        d2d1_brush->SetColor(gray_color);

        d2d1_render_target->DrawLine(
            D2D1::Point2F(single_kifu_rectf.left + 5.0f, single_kifu_rectf.bottom + 1.0f),
            D2D1::Point2F(single_kifu_rectf.right - 5.0f, single_kifu_rectf.bottom + 1.0f),
            d2d1_brush,
            1.0f
        );

        if (kifu_current_turn == turn) {
            d2d1_brush->SetColor(kifu_bg_color);
            d2d1_render_target->FillRectangle(single_kifu_rectf, d2d1_brush);
            d2d1_brush->SetColor(kifu_edge_color);
            d2d1_render_target->DrawRectangle(single_kifu_rectf, d2d1_brush, 3.0f);
            d2d1_brush->SetColor(black_color);
        }

        D2D1_RECT_F layout_rect = D2D1::RectF(
            kifu_area_rectf.left + config_ref.padding,
            y - config_ref.kifu_spacing * 0.05f,
            kifu_area_rectf.right - config_ref.padding,
            y + config_ref.kifu_spacing * 9.95f
        );
        d2d1_render_target->DrawText(
            std::to_wstring(turn + 1).c_str(),
            static_cast<UINT32>(std::to_wstring(turn + 1).size()),
            text_format_kifu,
            layout_rect,
            d2d1_brush
        );

        float cx = kifu_area_rectf.left + config_ref.kifu_turn_size_x + config_ref.kifu_spacing / 2;
        float cy = y + config_ref.kifu_spacing / 2;
        float r = config_ref.kifu_spacing * 0.4f;

        if (turn % 2 == 0) {
            d2d1_brush->SetColor(red_color);
            d2d1_render_target->DrawEllipse(
                D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r),
                d2d1_brush,
                2.0f
            );
        } else {
            d2d1_brush->SetColor(blue_color);
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

        d2d1_brush->SetColor(black_color);

        layout_rect.left += config_ref.kifu_turn_size_x + config_ref.kifu_spacing;
        std::wstring move_str = L"(" + std::to_wstring(move.x) + L", " + std::to_wstring(move.y) + L")";
        d2d1_render_target->DrawText(
            move_str.c_str(),
            static_cast<UINT32>(move_str.size()),
            text_format_kifu,
            layout_rect,
            d2d1_brush
        );
    }

    void window_main::draw_kifu_single_last(unsigned int turn) {
        float y = kifu_area_rectf.top + turn * config_ref.kifu_spacing - kifu_scroll_offset.y;

        if (y + config_ref.kifu_spacing < kifu_area_rectf.top || y > kifu_area_rectf.bottom) return;

        D2D1_RECT_F single_kifu_rectf = D2D1::RectF(
            kifu_area_rectf.left,
            y,
            kifu_area_rectf.right,
            y + config_ref.kifu_spacing
        );

        d2d1_brush->SetColor(gray_color);
        D2D1_RECT_F layout_rect = D2D1::RectF(
            kifu_area_rectf.left + config_ref.padding,
            y - config_ref.kifu_spacing * 0.05f,
            kifu_area_rectf.right - config_ref.padding,
            y + config_ref.kifu_spacing * 9.95f
        );
        d2d1_render_target->DrawText(
            std::to_wstring(turn + 1).c_str(),
            static_cast<UINT32>(std::to_wstring(turn + 1).size()),
            text_format_kifu,
            layout_rect,
            d2d1_brush
        );

        d2d1_brush->SetColor(black_color);
        layout_rect.left += config_ref.kifu_turn_size_x + config_ref.kifu_spacing;
        std::wstring move_str = L"投了";
        d2d1_render_target->DrawText(
            move_str.c_str(),
            static_cast<UINT32>(move_str.size()),
            text_format_kifu,
            layout_rect,
            d2d1_brush
        );
    }

    void window_main::draw_kifu() {
        if (!d2d1_render_target || !d2d1_brush) return;

        float kifu_spacing = config_ref.kifu_spacing;

        d2d1_render_target->DrawRectangle(
            kifu_area_rectf,
            d2d1_brush,
            2.0f
        );

        d2d1_render_target->PushAxisAlignedClip(
            kifu_area_clip_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        const auto &moves = current_kifu.data();
        for (unsigned int i = 0; i < moves.size(); i++) {
            draw_kifu_single(moves[i], i);
        }

        if (!moves.size() == 0 && !is_gaming) draw_kifu_single_last(static_cast<unsigned int>(moves.size()));

        d2d1_render_target->PopAxisAlignedClip();
    }

    void window_main::draw_config() {
        if (!d2d1_render_target || !d2d1_brush) return;

        d2d1_render_target->DrawRectangle(
            config_area_rectf,
            d2d1_brush,
            2.0f
        );

        d2d1_render_target->PushAxisAlignedClip(
            config_area_clip_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        D2D1_RECT_F text_layout = D2D1::RectF(
            config_area_rectf.left + config_ref.padding,
            config_area_rectf.top + config_ref.padding - config_scroll_offset.y,
            config_area_rectf.right - config_ref.padding,
            config_area_rectf.bottom - config_ref.padding - config_scroll_offset.y
        );

        std::wstring conf_str = utf8_to_utf16(current_kifu.config());

        d2d1_render_target->DrawText(
            conf_str.c_str(),
            static_cast<UINT32>(conf_str.size()),
            text_format_config,
            text_layout,
            d2d1_brush
        );

        d2d1_render_target->PopAxisAlignedClip();
    }

    void window_main::draw_game_control() {
        D2D1_COLOR_F color;
        switch (do_over_button_state) {
            case 0: {
                color = D2D1::ColorF(D2D1::ColorF::White);
                break;
            }
            case 1: {
                color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke);
                break;
            }
            case 2: {
                color = D2D1::ColorF(D2D1::ColorF::LightGray);
                break;
            }
        }
        d2d1_brush->SetColor(color);
        d2d1_render_target->FillRectangle(do_over_button_area_rectf, d2d1_brush);
        switch (resign_button_state) {
            case 0: {
                color = D2D1::ColorF(D2D1::ColorF::White);
                break;
            }
            case 1: {
                color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke);
                break;
            }
            case 2: {
                color = D2D1::ColorF(D2D1::ColorF::LightGray);
                break;
            }
        }
        d2d1_brush->SetColor(color);
        d2d1_render_target->FillRectangle(resign_button_area_rectf, d2d1_brush);
        d2d1_brush->SetColor(black_color);
        d2d1_render_target->DrawRectangle(do_over_button_area_rectf, d2d1_brush, 2.0f);
        d2d1_render_target->DrawRectangle(resign_button_area_rectf, d2d1_brush, 2.0f);
        d2d1_render_target->DrawText(
            L"待った", 3,
            text_format_game_control_button_label,
            &do_over_button_area_rectf,
            d2d1_brush
        );
        d2d1_render_target->DrawText(
            L"投了", 2,
            text_format_game_control_button_label,
            &resign_button_area_rectf,
            d2d1_brush
        );
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
        float total_height = static_cast<float>(moves.size() + 1) * config_ref.kifu_spacing + config_ref.padding;
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

    void window_main::set_kifu_scroll(float x, float y) {
        kifu_scroll_offset.x = x;
        kifu_scroll_offset.y = y;
    }

    D2D1_POINT_2F window_main::get_kifu_scroll() const {
        return kifu_scroll_offset;
    }

    void window_main::config_scroll(float dx, float dy) {
        config_scroll_offset.x += dx;
        config_scroll_offset.y += dy;

        std::wstring conf_str = utf8_to_utf16(current_kifu.config());
        Microsoft::WRL::ComPtr<IDWriteTextLayout> layout;
        HRESULT hr = d2d1_dwrite_factory->CreateTextLayout(
            conf_str.c_str(),
            static_cast<UINT32>(conf_str.size()),
            text_format_config,
            config_area_rectf.right - config_area_rectf.left,
            10000.0f,
            layout.GetAddressOf()
        );
        float text_height = 0.0f;
        if (SUCCEEDED(hr)) {
            DWRITE_TEXT_METRICS metrics;
            layout->GetMetrics(&metrics);
            text_height = metrics.height;
        }
        float view_height = config_area_rectf.bottom - config_area_rectf.top;

        if (config_scroll_offset.y < 0.0f) {
            config_scroll_offset.y = 0.0f;
        }
        if (text_height > view_height) {
            float max_offset = (text_height + config_ref.padding) - view_height;
            if (config_scroll_offset.y > max_offset) {
                config_scroll_offset.y = max_offset;
            }
        } else {
            config_scroll_offset.y = 0.0f;
        }
    }

    void window_main::set_config_scroll(float x, float y) {
        config_scroll_offset.x = x;
        config_scroll_offset.y = y;
    }

    D2D1_POINT_2F window_main::get_config_scroll() const {
        return config_scroll_offset;
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
            L"Hook-Mark_GUI_NewGame",
            LoadIconW(nullptr, MAKEINTRESOURCEW(32512))
        };
        return RegisterClassExW(&window_class);
    }

    void window_newgame::create_window(HWND handle_parent_window) {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_NewGame",
            L"新規対局 - Hook-Mark GUI",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            handle_parent_window, NULL, GetModuleHandle(nullptr), NULL
        );

        DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(handle_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void window_newgame::show_window(int show_command, float x, float y) {
        ShowWindow(handle_window, show_command);
        SetWindowPos(handle_window, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        UpdateWindow(handle_window);
    }

    void window_newgame::initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window) {
        config_ref = config;
        window_class.register_class();
        create_window(handle_parent_window);
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);

        handle_newgame_button = CreateWindowExW(NULL, L"BUTTON", L"対局開始", WS_CHILD | WS_VISIBLE, 10, 10, 300, 25, handle_window, reinterpret_cast<HMENU>(static_cast<uintptr_t>(ID_NEWGAME_BUTTON)), GetModuleHandleW(nullptr), nullptr);
        ShowWindow(handle_newgame_button, SW_SHOW);
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

        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,
            L"ja-JP",
            &text_format_default
        );
        if (FAILED(hr)) return false;

        return true;
    }

    void window_newgame::redraw() {
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));
        return;
    }

    void window_newgame::handle_exit() {
        show_window(SW_HIDE);
    }

    void window_newgame::release() {
        DestroyWindow(handle_window);
        text_format_default->Release();
    }

    wc_settings::wc_settings() : wc_base() {}

    ATOM wc_settings::register_class() {
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
            L"Hook-Mark_GUI_Settings",
            LoadIconW(nullptr, MAKEINTRESOURCEW(32512))
        };

        return RegisterClassExW(&window_class);
    }

    void window_settings::create_window(HWND handle_parent_window) {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_Settings",
            L"設定 - Hook-Mark GUI",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            handle_parent_window, NULL, GetModuleHandle(nullptr), NULL
        );

        DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(handle_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    bool window_settings::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
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

        d2d1_render_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

        hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            config_ref.kifu_spacing * 0.8f,
            L"ja-JP",
            &text_format_default
        );
        if (FAILED(hr)) return false;

        text_format_default->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        return true;
    }

    void window_settings::initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window) {
        config_ref = config;
        window_class.register_class();
        create_window(handle_parent_window);
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
    }

    void window_settings::update_rect() {
        GetClientRect(handle_window, &client_area_rect);
        client_area_rectf = rect_to_rectf(client_area_rect);
        settings_area_rectf = D2D1::RectF(
            client_area_rectf.left + config_ref.margin,
            client_area_rectf.top + config_ref.margin,
            client_area_rectf.right - config_ref.margin,
            client_area_rectf.bottom - config_ref.margin
        );
        settings_area_rect = rectf_to_rect(settings_area_rectf);
    }

    void window_settings::handle_exit() {
        show_window(SW_HIDE);
    }

    void window_settings::show_window(int show_command, float x, float y) {
        ShowWindow(handle_window, show_command);
        SetWindowPos(handle_window, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        UpdateWindow(handle_window);
    }

    void window_settings::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));

        d2d1_render_target->DrawRectangle(
            settings_area_rectf,
            d2d1_brush,
            2.0f
        );

        float center_x = settings_area_rectf.left + (settings_area_rectf.right - settings_area_rectf.left) / 2;

        d2d1_render_target->DrawLine(
            D2D1::Point2F(center_x, settings_area_rectf.top),
            D2D1::Point2F(center_x, settings_area_rectf.bottom),
            d2d1_brush,
            2.0f
        );

        d2d1_render_target->PushAxisAlignedClip(
            settings_area_rectf,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
        );

        int index = 0;
        for (auto it = settings_items.begin(); it != settings_items.end(); ++it, ++index) {
            float line_y = settings_area_rectf.top + settings_item_spacing * (index + 1);
            d2d1_render_target->DrawLine(
                D2D1::Point2F(settings_area_rectf.left, line_y),
                D2D1::Point2F(settings_area_rectf.right, line_y),
                d2d1_brush,
                1.0f
            );
            float item_y = settings_area_rectf.top + settings_item_spacing * index;
            d2d1_render_target->DrawText(
                it->first.c_str(),
                static_cast<UINT32>(it->first.size()),
                text_format_default,
                D2D1::RectF(
                    settings_area_rectf.left + config_ref.padding,
                    item_y + config_ref.padding,
                    center_x - config_ref.padding,
                    line_y + config_ref.padding
                ),
                d2d1_brush
            );
        }

        d2d1_render_target->PopAxisAlignedClip();
    }
}
