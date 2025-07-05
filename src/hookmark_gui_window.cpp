#include <hookmark_gui.hpp>

namespace hmgui {
    wc_main::wc_main() : wc_base() {}

    ATOM wc_main::register_class() {
        handle_instance = GetModuleHandleW(nullptr);

        window_class = {
            sizeof(WNDCLASSEXW),
            CS_HREDRAW | CS_VREDRAW,
            window_proc,
            0, 0,
            handle_instance,
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)),
            (HBRUSH)(COLOR_WINDOW + 1),
            nullptr,
            L"Hook-Mark_GUI_Main",
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON))
        };
        return RegisterClassExW(&window_class);
    }

    bool window_main::d2d1_update_text_format() {
        if (text_format_kifu) text_format_kifu->Release();
        if (text_format_label) text_format_label->Release();
        if (text_format_config) text_format_config->Release();
        if (text_format_button_label) text_format_button_label->Release();

        HRESULT hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            config_ref.kifu_spacing * 0.8f,
            L"ja-JP",
            &text_format_kifu
        );
        if (FAILED(hr)) return false;
        text_format_kifu->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
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
            L"Yu Gothic UI",
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
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,
            L"ja-JP",
            &text_format_button_label
        );
        if (FAILED(hr)) return false;

        text_format_button_label->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        text_format_button_label->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        return true;
    }

    bool window_main::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        d2d1_factory = i_d2d1_factory;
        d2d1_factory->AddRef();
        d2d1_dwrite_factory = i_d2d1_dwrite_factory;
        d2d1_dwrite_factory->AddRef();

        GetClientRect(handle_window, &client_area_rect);

        D2D1_SIZE_U size = D2D1::SizeU(client_area_rect.right - client_area_rect.left, client_area_rect.bottom - client_area_rect.top);

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

        hr = d2d1_render_target->CreateSolidColorBrush(black_color, &d2d1_brush);
        if (FAILED(hr)) return false;

        if (!d2d1_update_text_format()) return false;

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

        RECT rect_dwm = {};
        RECT rect_win = {};
        window_area_rect = {};
        if (SUCCEEDED(DwmIsCompositionEnabled(&is_dwm_enabled)) && is_dwm_enabled) {
            if (SUCCEEDED(DwmGetWindowAttribute(handle_window, DWMWA_EXTENDED_FRAME_BOUNDS, &rect_dwm, sizeof(RECT)))) {
                GetWindowRect(handle_window, &rect_win);
                int left_offset = rect_dwm.left - rect_win.left;
                int top_offset = rect_dwm.top - rect_win.top;
                int right_offset = rect_win.right - rect_dwm.right;
                int bottom_offset = rect_win.bottom - rect_dwm.bottom;
                window_area_rect.left = rect_dwm.left - left_offset;
                window_area_rect.top = rect_dwm.top - top_offset;
                window_area_rect.right = rect_dwm.right + right_offset;
                window_area_rect.bottom = rect_dwm.bottom + bottom_offset;
            } else {
                GetWindowRect(handle_window, &window_area_rect);
            }
        } else {
            GetWindowRect(handle_window, &window_area_rect);
        }
        window_area_rectf = rect_to_rectf(window_area_rect);

        config_ref.window_size_x = window_area_rectf.right - window_area_rectf.left;
        config_ref.window_size_y = window_area_rectf.bottom - window_area_rectf.top;
        config_ref.window_pos_x = window_area_rectf.left;
        config_ref.window_pos_y = window_area_rectf.top;
        grid_area_rectf = D2D1::RectF(
            config_ref.margin,
            config_ref.margin,
            config_ref.grid_size_x - config_ref.margin,
            config_ref.vertical_size - config_ref.margin
        );
        kifu_area_rectf = D2D1::RectF(
            config_ref.grid_size_x + config_ref.margin,
            config_ref.margin,
            config_ref.grid_size_x + config_ref.kifu_size_x - config_ref.margin,
            config_ref.vertical_size - config_ref.margin
        );
        config_area_rectf = D2D1::RectF(
            config_ref.grid_size_x + config_ref.kifu_size_x + config_ref.margin,
            config_ref.margin,
            client_area_rectf.right - config_ref.margin,
            config_ref.vertical_size - config_ref.margin
        );
        do_over_button.rectf = D2D1::RectF(
            config_ref.margin,
            config_ref.vertical_size + config_ref.margin,
            100.0f - config_ref.margin,
            config_ref.vertical_size + 40.0f - config_ref.margin
        );
        resign_button.rectf = D2D1::RectF(
            100.0f + config_ref.margin,
            config_ref.vertical_size + config_ref.margin,
            200.0f - config_ref.margin,
            config_ref.vertical_size + 40.0f - config_ref.margin
        );
        grid_area_rect = rectf_to_rect(grid_area_rectf);
        kifu_area_rect = rectf_to_rect(kifu_area_rectf);
        config_area_rect = rectf_to_rect(config_area_rectf);
        do_over_button.rect = rectf_to_rect(do_over_button.rectf);
        resign_button.rect = rectf_to_rect(resign_button.rectf);
        grid_area_clip_rectf = cliped_rectf(grid_area_rectf);
        kifu_area_clip_rectf = cliped_rectf(kifu_area_rectf);
        config_area_clip_rectf = cliped_rectf(config_area_rectf);
        boundary_grid = config_ref.grid_size_x;
        boundary_kifu = config_ref.grid_size_x + config_ref.kifu_size_x;
        boundary_kifu_turn = config_ref.grid_size_x + config_ref.kifu_turn_size_x;
    }

    void window_main::update_scroll_speed() {
        scroll_speed = config_ref.grid_spacing / 0.3f;
    }

    void window_main::initialize_scroll() {
        grid_scroll_offset = D2D1::Point2F(-(config_ref.grid_size_x - config_ref.margin * 2) / 2 + config_ref.grid_spacing / 2, (config_ref.vertical_size - config_ref.margin * 2) / 2 - config_ref.grid_spacing / 2);
        kifu_scroll_offset = D2D1::Point2F();
        config_scroll_offset = D2D1::Point2F();
    }

    void window_main::initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        initialize_scroll();
        handle_instance = GetModuleHandleW(nullptr);
        window_class.register_class();
        create_window();
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
        do_over_button.initialize(d2d1_factory, d2d1_dwrite_factory, d2d1_render_target, d2d1_brush, text_format_button_label, L"待った");
        resign_button.initialize(d2d1_factory, d2d1_dwrite_factory, d2d1_render_target, d2d1_brush, text_format_button_label, L"投了");
        update_rect();
    }

    void window_main::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_Main",
            L"Hook-Mark GUI",
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            static_cast<int>(config_ref.window_pos_x), static_cast<int>(config_ref.window_pos_y),
            static_cast<int>(config_ref.window_size_x), static_cast<int>(config_ref.window_size_y),
            nullptr, nullptr, handle_instance, this
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
        d2d1_render_target->Clear(white_color);
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
                grid_area_rectf.bottom - config_ref.label_size - 5.0f,
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

            add_label_size(static_cast<int>(txt.size()));

            D2D1_RECT_F layout = D2D1::RectF(
                grid_area_rectf.left + config_ref.padding,
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
            if (-1 < kifu_current_turn) {
                int draw_turn = kifu_current_turn;
                hm::pos p = current_kifu[draw_turn];
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
        }

        d2d1_brush->SetColor(black_color);
    }

    void window_main::draw_kifu_single(const hm::pos &move, unsigned int turn) {
        float y = kifu_area_rectf.top + (turn + 1) * config_ref.kifu_spacing - kifu_scroll_offset.y;

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
            d2d1_render_target->DrawRectangle(single_kifu_rectf, d2d1_brush, 1.0f);
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

    void window_main::draw_kifu_single_first() {
        float y = kifu_area_rectf.top - kifu_scroll_offset.y;

        if (y + config_ref.kifu_spacing < kifu_area_rectf.top || y > kifu_area_rectf.bottom) return;

        D2D1_RECT_F single_kifu_rectf = D2D1::RectF(
            kifu_area_rectf.left,
            y,
            kifu_area_rectf.right,
            y + config_ref.kifu_spacing
        );

        if (kifu_current_turn == -1) {
            d2d1_brush->SetColor(kifu_bg_color);
            d2d1_render_target->FillRectangle(single_kifu_rectf, d2d1_brush);
            d2d1_brush->SetColor(kifu_edge_color);
            d2d1_render_target->DrawRectangle(single_kifu_rectf, d2d1_brush, 1.0f);
            d2d1_brush->SetColor(black_color);
        }

        d2d1_brush->SetColor(gray_color);
        d2d1_render_target->DrawLine(
            D2D1::Point2F(single_kifu_rectf.left + 5.0f, single_kifu_rectf.bottom + 1.0f),
            D2D1::Point2F(single_kifu_rectf.right - 5.0f, single_kifu_rectf.bottom + 1.0f),
            d2d1_brush,
            1.0f
        );

        D2D1_RECT_F layout_rect = D2D1::RectF(
            kifu_area_rectf.left + config_ref.padding,
            y - config_ref.kifu_spacing * 0.05f,
            kifu_area_rectf.right - config_ref.padding,
            y + config_ref.kifu_spacing * 9.95f
        );

        if (kifu_current_turn == -1) {
            d2d1_brush->SetColor(black_color);
        }
        d2d1_render_target->DrawText(
            std::to_wstring(0).c_str(),
            static_cast<UINT32>(std::to_wstring(0).size()),
            text_format_kifu,
            layout_rect,
            d2d1_brush
        );

        d2d1_brush->SetColor(black_color);
        layout_rect.left += config_ref.kifu_turn_size_x + config_ref.kifu_spacing;
        std::wstring move_str = L"開始局面";
        d2d1_render_target->DrawText(
            move_str.c_str(),
            static_cast<UINT32>(move_str.size()),
            text_format_kifu,
            layout_rect,
            d2d1_brush
        );
    }

    void window_main::draw_kifu_single_last(unsigned int turn) {
        float y = kifu_area_rectf.top + (turn + 1) * config_ref.kifu_spacing - kifu_scroll_offset.y;

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

        draw_kifu_single_first();

        const auto &moves = current_kifu.data();
        for (int i = 0; i < moves.size(); i++) {
            draw_kifu_single(moves[i], i);
        }

        if (current_kifu.is_resigned()) draw_kifu_single_last(static_cast<unsigned int>(moves.size()));

        d2d1_render_target->DrawLine(
            D2D1::Point2F(config_ref.grid_size_x + config_ref.kifu_turn_size_x, config_ref.margin),
            D2D1::Point2F(config_ref.grid_size_x + config_ref.kifu_turn_size_x, config_ref.vertical_size - config_ref.margin),
            d2d1_brush,
            2.0f
        );

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

        hm::kifu_config kc = current_kifu.config_struct();
        std::wstring str;
        if (!(kc.first.empty() && kc.second.empty())) {
            str = L"先手: " + utf8_to_utf16(kc.first) + L"\n後手: " + utf8_to_utf16(kc.second) + L"\n";
        }
        std::wstring conf_str = str + utf8_to_utf16(current_kifu.config());

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
        do_over_button.redraw();
        resign_button.redraw();
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
        float total_height = static_cast<float>(moves.size() + 1 + static_cast<int>(!is_gaming)) * config_ref.kifu_spacing + config_ref.padding;
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

    void window_main::update_title() {
        std::filesystem::path path(current_kifu_path);
        std::wstring filename = path.filename().wstring();
        std::wstring title = (current_kifu_path.empty() ? L"無題" : filename) + (kifu_saved ? L"" : L" *") + L" - Hook-Mark GUI" + L"\0";
        SetWindowTextW(handle_window, title.c_str());
    }

    bool window_main::save_as_handler() {
        std::wstring result;
        if (!current_kifu.is_resigned()) {
            int ret = check_resign(handle_window);
            switch (ret) {
                case 0: {
                    current_kifu.resign();
                    break;
                }
                case 1:
                case 2: {
                    return false;
                }
            }
        }
        show_file_save_dialog(result);
        current_kifu_path = result;
        if (result.empty()) {
            return true;
        }
        try {
            current_kifu.kifu_save(result);
            kifu_saved = true;
            if (is_editing) {
                SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_BOARD, NULL), MAKELPARAM(NULL, NULL));
            }
        }
        catch (const std::exception &e) {
            MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(),
                        L"エラー", MB_OK | MB_ICONERROR);
            return true;
        }
        update_title();
        return false;
    }

    bool window_main::overwrite_save_handler() {
        if (current_kifu_path.empty()) {
            if (save_as_handler()) {
                return true;
            }
            return false;
        }
        else {
            if (!current_kifu.is_resigned()) {
                int ret = check_resign(handle_window);
                switch (ret) {
                    case 0: {
                        current_kifu.resign();
                        break;
                    }
                    case 1:
                    case 2: {
                        return false;
                    }
                }
            }
            try {
                current_kifu.kifu_save(current_kifu_path);
                kifu_saved = true;
            }
            catch (const std::exception &e) {
                MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(),
                            L"エラー", MB_OK | MB_ICONERROR);
                return true;
            }
        }
        return false;
    }

    wc_newgame::wc_newgame() : wc_base() {}

    ATOM wc_newgame::register_class() {
        handle_instance = GetModuleHandleW(nullptr);

        window_class = {
            sizeof(WNDCLASSEXW),
            CS_HREDRAW | CS_VREDRAW,
            window_proc,
            0, 0,
            handle_instance,
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)),
            (HBRUSH)(COLOR_WINDOW + 1),
            nullptr,
            L"Hook-Mark_GUI_NewGame",
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON))
        };
        return RegisterClassExW(&window_class);
    }

    void window_newgame::create_window(HWND handle_parent_window) {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_NewGame",
            L"新規対局 - Hook-Mark GUI",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            handle_parent_window, NULL, handle_instance, NULL
        );

        DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(handle_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void window_newgame::show_window(int show_command, float x, float y) {
        SetWindowPos(handle_window, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        ShowWindow(handle_window, show_command);

        if (players_name_edit.first) DestroyWindow(players_name_edit.first);
        if (players_name_edit.second) DestroyWindow(players_name_edit.second);

        players_name_edit.first = CreateWindowExW(
            0,
            L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            50, 10, client_area_rect.right / 2 - 65, 30,
            handle_window, NULL, handle_instance, NULL
        );
        players_name_edit.second = CreateWindowExW(
            0,
            L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            client_area_rect.right / 2 + 45, 10, client_area_rect.right / 2 - 60, 30,
            handle_window, NULL, handle_instance, NULL
        );
        if (!handle_font) {
            handle_font = CreateFontW(
                18,
                0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Yu Gothic UI"
            );
        }

        SetWindowFont(players_name_edit.first, handle_font, FALSE);
        SetWindowFont(players_name_edit.second, handle_font, FALSE);

        SetWindowSubclass(players_name_edit.first, handle_edit_name_message, 1, 0);
        SetWindowSubclass(players_name_edit.second, handle_edit_name_message, 1, 0);

        ShowWindow(players_name_edit.first, SW_SHOW);
        ShowWindow(players_name_edit.second, SW_SHOW);

        UpdateWindow(handle_window);
    }

    void window_newgame::initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window) {
        newgame_config_area_rect.resize(newgame_config_size);
        newgame_config_area_rectf.resize(newgame_config_size);
        newgame_config_state.resize(newgame_config_size);
        handle_instance = GetModuleHandleW(nullptr);
        window_class.register_class();
        create_window(handle_parent_window);
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
        newgame_button.initialize(d2d1_factory, d2d1_dwrite_factory, d2d1_render_target, d2d1_brush, text_format_button_label, L"対局開始");
        update_rect();
    }

    bool window_newgame::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        d2d1_factory = i_d2d1_factory;
        d2d1_factory->AddRef();
        d2d1_dwrite_factory = i_d2d1_dwrite_factory;
        d2d1_dwrite_factory->AddRef();

        RECT client_area_rect;
        GetClientRect(handle_window, &client_area_rect);

        D2D1_SIZE_U size = D2D1::SizeU(client_area_rect.right - client_area_rect.left, client_area_rect.bottom - client_area_rect.top);

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

        hr = d2d1_render_target->CreateSolidColorBrush(black_color, &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,
            L"ja-JP",
            &text_format_default
        );
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            15.0f,
            L"ja-JP",
            &text_format_button_label
        );
        if (FAILED(hr)) return false;

        text_format_button_label->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        text_format_button_label->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

        text_format_default->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        return SUCCEEDED(hr);
    }

    void window_newgame::update_rect() {
        GetClientRect(handle_window, &client_area_rect);
        client_area_rectf = rect_to_rectf(client_area_rect);
        newgame_button.rectf = D2D1::RectF(
            client_area_rectf.right / 2 - 50.0f + config_ref.margin,
            client_area_rectf.bottom - 40.0f + config_ref.margin,
            client_area_rectf.right / 2 + 50.0f - config_ref.margin,
            client_area_rectf.bottom - config_ref.margin
        );
        newgame_button.rect = rectf_to_rect(newgame_button.rectf);
        for (int i = 0; i < newgame_config_size; i++) {
            newgame_config_area_rectf[i] = D2D1::RectF(
                8.0f,
                68.0f + i * 25.0f,
                22.0f,
                82.0f + i * 25.0f
            );
            newgame_config_area_rect[i] = rectf_to_rect(newgame_config_area_rectf[i]);
        }
    }

    void window_newgame::redraw() {
        d2d1_render_target->Clear(white_color);

        d2d1_render_target->DrawText(
            L"先手", 2u,
            text_format_default,
            D2D1::RectF(5.0f, 10.0f, 300.0f, 35.0f),
            d2d1_brush
        );

        d2d1_render_target->DrawText(
            L"後手", 2u,
            text_format_default,
            D2D1::RectF(290.0f, 10.0f, 595.0f, 35.0f),
            d2d1_brush
        );

        for (int i = 0; i < newgame_config_size; i++) {
            d2d1_render_target->DrawEllipse(
                D2D1::Ellipse({ 15.0f, 75.0f + i * 25.0f }, 7.0f, 7.0f),
                d2d1_brush,
                2.0f
            );

            if (newgame_config_state[i]) {
                d2d1_render_target->FillEllipse(
                    D2D1::Ellipse({ 15.0f, 75.0f + i * 25.0f }, 4.0f, 4.0f),
                    d2d1_brush
                );
            }

            d2d1_render_target->DrawText(
                newgame_config_keys[i].c_str(),
                static_cast<UINT32>(newgame_config_keys[i].size()),
                text_format_default,
                D2D1::RectF(
                    30.0f,
                    68.0f + i * 25.0f,
                    570.0f,
                    82.0f + i * 25.0f
                ),
                d2d1_brush
            );
        }

        newgame_button.redraw();
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
        handle_instance = GetModuleHandleW(nullptr);

        window_class = {
            sizeof(WNDCLASSEXW),
            CS_HREDRAW | CS_VREDRAW,
            window_proc,
            0, 0,
            handle_instance,
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)),
            (HBRUSH)(COLOR_WINDOW + 1),
            nullptr,
            L"Hook-Mark_GUI_Settings",
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON))
        };

        return RegisterClassExW(&window_class);
    }

    void window_settings::create_window(HWND handle_parent_window) {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_Settings",
            L"設定 - Hook-Mark GUI",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            handle_parent_window, NULL, handle_instance, NULL
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

        GetClientRect(handle_window, &client_area_rect);

        D2D1_SIZE_U size = D2D1::SizeU(client_area_rect.right - client_area_rect.left, client_area_rect.bottom - client_area_rect.top);

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

        hr = d2d1_render_target->CreateSolidColorBrush(black_color, &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            settings_item_spacing * 0.7f,
            L"ja-JP",
            &text_format_default
        );
        if (FAILED(hr)) return false;

        text_format_default->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        return true;
    }

    void window_settings::initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window) {
        handle_instance = GetModuleHandleW(nullptr);
        window_class.register_class();
        edit_controls.resize(settings_item_keys.size());
        create_window(handle_parent_window);
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
    }

    void window_settings::create_edit_controls() {
        if (!edit_controls.empty()) {
            for (HWND handle_edit_control : edit_controls) {
                DestroyWindow(handle_edit_control);
            }
            edit_controls.clear();
        }
        edit_controls.resize(settings_item_keys.size());
        float center_x = settings_area_rectf.left + (settings_area_rectf.right - settings_area_rectf.left) / 2;
        HFONT handle_font = CreateFontW(
            static_cast<int>(settings_item_spacing * 0.7f),
            0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Yu Gothic UI"
        );
        for (int i = 0; i < settings_item_keys.size(); i++) {
            HWND edit_handle = CreateWindowExW(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE,
                static_cast<int>(center_x) + 1, static_cast<int>(config_ref.margin + settings_item_spacing * i) + 1,
                static_cast<int>((settings_area_rectf.right - settings_area_rectf.left) / 2) - 2, static_cast<int>(settings_item_spacing) - 2,
                handle_window, nullptr, handle_instance, nullptr
            );
            ShowWindow(edit_handle, SW_SHOW);
            SetWindowFont(edit_handle, handle_font, TRUE);
            SetWindowSubclass(edit_handle, handle_config_edit_message, 2, 0);
            edit_controls[i] = edit_handle;
        }

        for (int i = 0; i < settings_item_references.size(); i++) {
            SetWindowTextW(edit_controls[i], std::to_wstring(settings_item_references[i]).c_str());
        }
    }

    void window_settings::update_config() {
        wchar_t buffer[256];

        for (int i = 0; i < settings_item_references.size(); i++) {
            GetWindowTextW(edit_controls[i], buffer, 256);
            try {
                settings_item_references[i].get() = std::stof(buffer);
            } catch (...) {
                settings_item_references[i].get() = settings_item_default[i];
            }
        }
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
        update_config();
        show_window(SW_HIDE);
    }

    void window_settings::release() {
        DestroyWindow(handle_window);
        for (auto &edit_handle : edit_controls) {
            DestroyWindow(edit_handle);
        }
        text_format_default->Release();
        d2d1_brush->Release();
        d2d1_render_target->Release();
    }

    void window_settings::show_window(int show_command, float x, float y) {
        SetWindowPos(handle_window, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        ShowWindow(handle_window, show_command);
        if (show_command == SW_SHOW) {
            create_edit_controls();
        }
        UpdateWindow(handle_window);
    }

    void window_settings::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(white_color);

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
        for (const auto &item_key : settings_item_keys) {
            float line_y = settings_area_rectf.top + settings_item_spacing * (index + 1);
            d2d1_render_target->DrawLine(
                D2D1::Point2F(settings_area_rectf.left, line_y),
                D2D1::Point2F(settings_area_rectf.right, line_y),
                d2d1_brush,
                1.0f
            );
            float item_y = settings_area_rectf.top + settings_item_spacing * index;
            d2d1_render_target->DrawText(
                item_key.c_str(),
                static_cast<UINT32>(item_key.size()),
                text_format_default,
                D2D1::RectF(
                    settings_area_rectf.left + config_ref.padding,
                    item_y,
                    center_x - config_ref.padding,
                    line_y
                ),
                d2d1_brush
            );

            index++;
        }

        d2d1_render_target->PopAxisAlignedClip();
    }

    wc_version::wc_version() : wc_base() {}

    ATOM wc_version::register_class() {
        handle_instance = GetModuleHandleW(nullptr);

        window_class = {
            sizeof(WNDCLASSEXW),
            CS_HREDRAW | CS_VREDRAW,
            window_proc,
            0, 0,
            handle_instance,
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)),
            (HBRUSH)(COLOR_WINDOW + 1),
            nullptr,
            L"Hook-Mark_GUI_Version",
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON))
        };

        return RegisterClassExW(&window_class);
    }

    void window_version::create_window(HWND handle_parent_window) {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_Version",
            L"バージョン情報 - Hook-Mark GUI",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            handle_parent_window, NULL, handle_instance, NULL
        );

        DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(handle_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void window_version::initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window) {
        handle_instance = GetModuleHandleW(nullptr);
        window_class.register_class();
        create_window(handle_parent_window);
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
    }

    bool window_version::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        d2d1_factory = i_d2d1_factory;
        d2d1_factory->AddRef();
        d2d1_dwrite_factory = i_d2d1_dwrite_factory;
        d2d1_dwrite_factory->AddRef();

        RECT client_area_rect;
        GetClientRect(handle_window, &client_area_rect);

        D2D1_SIZE_U size = D2D1::SizeU(client_area_rect.right - client_area_rect.left, client_area_rect.bottom - client_area_rect.top);

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

        hr = d2d1_render_target->CreateSolidColorBrush(black_color, &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,
            L"ja-JP",
            &text_format_default
        );

        return SUCCEEDED(hr);
    }

    void window_version::show_window(int show_command, float x, float y) {
        SetWindowPos(handle_window, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        ShowWindow(handle_window, show_command);
        UpdateWindow(handle_window);
    }

    void window_version::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(white_color);

        d2d1_render_target->DrawText(
            HOOKMARK_GUI_VERSION_TEXT,
            static_cast<UINT32>(std::wcslen(HOOKMARK_GUI_VERSION_TEXT)),
            text_format_default,
            D2D1::RectF(10.0f, 10.0f, 590.0f, 590.0f),
            d2d1_brush
        );
    }

    void window_version::handle_exit() {
        show_window(SW_HIDE);
    }

    void window_version::release() {
        DestroyWindow(handle_window);
        text_format_default->Release();
        d2d1_brush->Release();
        d2d1_render_target->Release();
    }

    wc_sep_board::wc_sep_board() : wc_base() {}

    ATOM wc_sep_board::register_class() {
        handle_instance = GetModuleHandleW(nullptr);

        handle_instance = GetModuleHandleW(nullptr);
        window_class = {
            sizeof(WNDCLASSEXW),
            CS_HREDRAW | CS_VREDRAW,
            window_proc,
            0, 0,
            handle_instance,
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON)),
            LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)),
            (HBRUSH)(COLOR_WINDOW + 1),
            nullptr,
            L"Hook-Mark_GUI_SepBoard",
            LoadIconW(handle_instance, MAKEINTRESOURCEW(IDI_HOOKMARK_GUI_ICON))
        };

        return RegisterClassExW(&window_class);
    }

    void window_sep_board::add_label_size(int i) {
        if (label_width.size() <= i) {
            label_width.resize(i + 1, 0.0f);
            label_height.resize(i + 1, 0.0f);
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

    void window_sep_board::initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        config_ref = config;
        handle_instance = GetModuleHandleW(nullptr);
        window_class.register_class();
        create_window();
        d2d1_initialize(i_d2d1_factory, i_d2d1_dwrite_factory);
        update_rect();
    }

    void window_sep_board::update_rect() {
        GetClientRect(handle_window, &client_area_rect);
        client_area_rectf = rect_to_rectf(client_area_rect);
        grid_area_rectf = D2D1::RectF(
            client_area_rectf.left + config_ref.margin,
            client_area_rectf.top + config_ref.margin,
            client_area_rectf.right - config_ref.margin,
            client_area_rectf.bottom - config_ref.margin
        );
        grid_area_rect = rectf_to_rect(grid_area_rectf);
        grid_area_clip_rectf = cliped_rectf(grid_area_rectf);
    }

    bool window_sep_board::d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory) {
        d2d1_factory = i_d2d1_factory;
        d2d1_factory->AddRef();
        d2d1_dwrite_factory = i_d2d1_dwrite_factory;
        d2d1_dwrite_factory->AddRef();

        GetClientRect(handle_window, &client_area_rect);

        D2D1_SIZE_U size = D2D1::SizeU(client_area_rect.right - client_area_rect.left, client_area_rect.bottom - client_area_rect.top);

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

        hr = d2d1_render_target->CreateSolidColorBrush(black_color, &d2d1_brush);
        if (FAILED(hr)) return false;

        hr = d2d1_dwrite_factory->CreateTextFormat(
            L"Yu Gothic UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            config_ref.label_size,
            L"ja-JP",
            &text_format_label
        );

        return SUCCEEDED(hr);
    }

    void window_sep_board::create_window() {
        handle_window = CreateWindowExW(
            0,
            L"Hook-Mark_GUI_SepBoard",
            L"盤面 - Hook-Mark GUI",
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            CW_USEDEFAULT, CW_USEDEFAULT,
            600, 600,
            NULL, NULL, handle_instance, NULL
        );

        DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(handle_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

        SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void window_sep_board::show_window(int show_command, float x, float y, hm::board_state i_board, hm::kifu_ver1 i_kifu, unsigned int i_kifu_current_turn) {
        board = i_board;
        current_kifu = i_kifu;
        kifu_current_turn = i_kifu_current_turn;
        SetWindowPos(handle_window, nullptr, static_cast<int>(x), static_cast<int>(y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        ShowWindow(handle_window, show_command);
        UpdateWindow(handle_window);
    }

    void window_sep_board::redraw() {
        if (!d2d1_render_target || !d2d1_brush) return;
        d2d1_render_target->Clear(white_color);
        draw_grid();
    }

    void window_sep_board::update_scroll_speed() {
        scroll_speed = config_ref.grid_spacing / 0.3f;
    }

    void window_sep_board::draw_grid() {
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
                grid_area_rectf.bottom - config_ref.label_size - 5.0f,
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

            add_label_size(static_cast<int>(txt.size()));

            D2D1_RECT_F layout = D2D1::RectF(
                grid_area_rectf.left + config_ref.padding,
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

    void window_sep_board::draw_board() {
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
            if (-1 < kifu_current_turn) {
                int draw_turn = kifu_current_turn;
                hm::pos p = current_kifu[draw_turn];
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
        }

        d2d1_brush->SetColor(black_color);
    }

    void window_sep_board::handle_exit() {
        show_window(SW_HIDE);
    }

    void window_sep_board::release() {
        DestroyWindow(handle_window);
        d2d1_brush->Release();
        d2d1_render_target->Release();
        text_format_label->Release();
    }

    void window_sep_board::grid_scroll(float dx, float dy) {
        grid_scroll_offset.x += dx;
        grid_scroll_offset.y += dy;
    }

    void window_sep_board::set_grid_scroll(float x, float y) {
        grid_scroll_offset.x = x;
        grid_scroll_offset.y = y;
    }

    D2D1_POINT_2F window_sep_board::get_grid_scroll() const {
        return grid_scroll_offset;
    }
}
