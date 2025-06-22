#include <hookmark.hpp>
#include <hookmark_gui_window.hpp>

hmgui::window_conf grobal_config;

hmgui::window_main main_window(grobal_config);
hmgui::wc_main main_window_class;
hmgui::window_newgame newgame_window(grobal_config);
hmgui::wc_newgame newgame_window_class;
hmgui::window_settings settings_window(grobal_config);
hmgui::wc_settings settings_window_class;
hmgui::window_version version_window;
hmgui::wc_version version_window_class;
hmgui::menu_main main_menu;
hmgui::menu_item_popup  main_menu_file,
                        main_menu_edit,
                        main_menu_view,
                        main_menu_game,
                        main_menu_help;
hmgui::menu_item        main_menu_file_create_new(ID_MENU_FILE_CREATE_NEW),
                        main_menu_file_open(ID_MENU_FILE_OPEN),
                        main_menu_file_overwrite_save(ID_MENU_FILE_OVERWRITE_SAVE),
                        main_menu_file_save_as(ID_MENU_FILE_SAVE_AS),
                        main_menu_file_close(ID_MENU_FILE_CLOSE),
                        main_menu_file_settings(ID_MENU_FILE_SETTINGS),
                        main_menu_file_exit(ID_MENU_FILE_EXIT),
                        main_menu_edit_move_forward(ID_MENU_EDIT_MOVE_FORWARD),
                        main_menu_edit_step_back(ID_MENU_EDIT_STEP_BACK),
                        main_menu_edit_jump_to_first_move(ID_MENU_EDIT_JUMP_TO_FIRST_MOVE),
                        main_menu_edit_comment(ID_MENU_EDIT_COMMENT),
                        main_menu_edit_kifu_info(ID_MENU_EDIT_KIFU_INFO),
                        main_menu_view_board_scroll_reset(ID_MENU_VIEW_BOARD_SCROLL_RESET),
                        main_menu_view_board_separate_window(ID_MENU_VIEW_BOARD_SEPARATE_WINDOW),
                        main_menu_game_new(ID_MENU_GAME_NEW),
                        main_menu_game_do_over(ID_MENU_GAME_DO_OVER),
                        main_menu_game_resign(ID_MENU_GAME_RESIGN),
                        main_menu_help_version(ID_MENU_HELP_VERSION);

ID2D1Factory *grobal_d2d1_factory;
IDWriteFactory *grobal_d2d1_dwrite_factory;

bool ctrl_down = false;
bool key_held[256] = {};
UINT_PTR timer_id = 1;

namespace hmgui {
    void update_title() {
        if (main_window.current_kifu_path.empty()) {
            SetWindowTextW(main_window, L"Hook-Mark GUI");
        } else {
            std::filesystem::path path(main_window.current_kifu_path);
            std::wstring filename = path.filename().wstring();
            std::wstring title = L"Hook-Mark GUI - " + filename + L"\0";
            SetWindowTextW(main_window, title.c_str());
        }
    }

    float to_float(const std::string &str) {
        try {
            return std::stof(str);
        }
        catch (...) {
            return 0.0f;
        }
    }

    bool to_bool(const std::string &str) {
        return (str == "1" || str == "true" || str == "TRUE");
    }

    void load_config() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::filesystem::path exe_path(utf16_to_utf8(path));
        std::filesystem::path dir = exe_path.parent_path();
        std::string init_path = (dir / "config" / "init.cfg").string();
        std::string default_path = (dir / "config" / "default.cfg").string();
        std::unordered_map<std::string, std::string> um_config;
        if (std::filesystem::exists(default_path)) {
            load_config_single(default_path, um_config);
        }
        if (std::filesystem::exists(init_path)) {
            load_config_single(init_path, um_config);
        }
        if (um_config.count("window_pos_x")) grobal_config.window_pos_x = to_float(um_config.at("window_pos_x"));
        if (um_config.count("window_pos_y")) grobal_config.window_pos_y = to_float(um_config.at("window_pos_y"));
        if (um_config.count("margin")) grobal_config.margin = to_float(um_config.at("margin"));
        if (um_config.count("padding")) grobal_config.padding = to_float(um_config.at("padding"));
        if (um_config.count("window_size_x")) grobal_config.window_size_x = to_float(um_config.at("window_size_x"));
        if (um_config.count("window_size_y")) grobal_config.window_size_y = to_float(um_config.at("window_size_y"));
        if (um_config.count("grid_spacing")) grobal_config.grid_spacing = to_float(um_config.at("grid_spacing"));
        if (um_config.count("grid_size_x")) grobal_config.grid_size_x = to_float(um_config.at("grid_size_x"));
        if (um_config.count("kifu_size_x")) grobal_config.kifu_size_x = to_float(um_config.at("kifu_size_x"));
        if (um_config.count("kifu_turn_size_x")) grobal_config.kifu_turn_size_x = to_float(um_config.at("kifu_turn_size_x"));
        if (um_config.count("grid_and_kifu_size_y")) grobal_config.grid_and_kifu_size_y = to_float(um_config.at("grid_and_kifu_size_y"));
        if (um_config.count("open_file")) grobal_config.open_file = um_config.at("open_file");
        if (um_config.count("label_size")) grobal_config.label_size = to_float(um_config.at("label_size"));
        if (um_config.count("first_name")) grobal_config.first_name = um_config.at("first_name");
        if (um_config.count("second_name")) grobal_config.second_name = um_config.at("second_name");
        if (um_config.count("first_time")) grobal_config.first_time = to_float(um_config.at("first_time"));
        if (um_config.count("second_time")) grobal_config.second_time = to_float(um_config.at("second_time"));
        if (um_config.count("first_countdown")) grobal_config.first_countdown = to_float(um_config.at("first_countdown"));
        if (um_config.count("second_countdown")) grobal_config.second_countdown = to_float(um_config.at("second_countdown"));
        if (um_config.count("lose_time_runs_out")) grobal_config.lose_time_runs_out = to_bool(um_config.at("lose_time_runs_out"));
        if (um_config.count("kifu_spacing")) grobal_config.kifu_spacing = to_float(um_config.at("kifu_spacing"));
    }

    void save_config() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::filesystem::path exe_path(utf16_to_utf8(path));
        std::filesystem::path dir = exe_path.parent_path();
        std::ofstream file(dir / "config" / "init.cfg");

        if (!file.is_open()) return;

        file << "[Hook-Mark GUI Window Initialize Config]\n\n#Main window\n";
        file << "window_pos_x = " << grobal_config.window_pos_x << "\n";
        file << "window_pos_y = " << grobal_config.window_pos_y << "\n";
        file << "margin = " << grobal_config.margin << "\n";
        file << "padding = " << grobal_config.padding << "\n";
        file << "window_size_x = " << grobal_config.window_size_x << "\n";
        file << "window_size_y = " << grobal_config.window_size_y << "\n";
        file << "grid_spacing = " << grobal_config.grid_spacing << "\n";
        file << "kifu_spacing = " << grobal_config.kifu_spacing << "\n";
        file << "grid_size_x = " << grobal_config.grid_size_x << "\n";
        file << "kifu_size_x = " << grobal_config.kifu_size_x << "\n";
        file << "kifu_turn_size_x = " << grobal_config.kifu_turn_size_x << "\n";
        file << "grid_and_kifu_size_y = " << grobal_config.grid_and_kifu_size_y << "\n";
        file << "open_file = " << grobal_config.open_file << "\n";
        file << "label_size = " << grobal_config.label_size << "\n\n#New game window\n";
        file << "first_name = " << grobal_config.first_name << "\n";
        file << "second_name = " << grobal_config.second_name << "\n";
        file << "first_time = " << grobal_config.first_time << "\n";
        file << "second_time = " << grobal_config.second_time << "\n";
        file << "first_countdown = " << grobal_config.first_countdown << "\n";
        file << "second_countdown = " << grobal_config.second_countdown << "\n";
        file << "lose_time_runs_out = " << (grobal_config.lose_time_runs_out ? "true" : "false") << "\n";

        file.close();
    }

    int check_nosave() {
        int ret = MessageBoxW(
            main_window,
            L"変更内容が保存されていません。保存しますか？",
            L"確認",
            MB_ICONWARNING | MB_YESNOCANCEL | MB_DEFBUTTON1
        );
        if (ret == IDYES) return 0;
        else if (ret == IDNO) return 1;
        else return 2;
    }

    void menu_main::create_menu() {
        handle_menu = CreateMenu();
        if (handle_menu) {
            AppendMenuW(handle_menu, MF_POPUP, main_menu_file, L"ファイル(&F)");
            AppendMenuW(handle_menu, MF_POPUP, main_menu_edit, L"編集(&E)");
            AppendMenuW(handle_menu, MF_POPUP, main_menu_view, L"表示(&V)");
            AppendMenuW(handle_menu, MF_POPUP, main_menu_game, L"対局(&G)");
            AppendMenuW(handle_menu, MF_POPUP, main_menu_help, L"ヘルプ(&H)");

            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_create_new, L"新規作成(&N)");
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_open, L"開く(&O)");
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_overwrite_save, L"上書き保存(&S)");
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_save_as, L"名前を付けて保存(&A)");
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_close, L"閉じる");
            AppendMenuW(main_menu_file, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_settings, L"設定(&S)");
            AppendMenuW(main_menu_file, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_exit, L"終了");

            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_move_forward, L"一手進む");
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_step_back, L"一手戻る");
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_jump_to_first_move, L"初手に戻る");
            AppendMenuW(main_menu_edit, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_comment, L"コメントを編集");
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_kifu_info, L"棋譜情報を編集");

            AppendMenuW(main_menu_view, MF_STRING, main_menu_view_board_scroll_reset, L"盤面のスクロールをリセット");
            AppendMenuW(main_menu_view, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_view, MF_STRING, main_menu_view_board_separate_window, L"盤面を別ウィンドウで表示");

            AppendMenuW(main_menu_game, MF_STRING, main_menu_game_new, L"新規対局");
            AppendMenuW(main_menu_game, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_game, MF_STRING, main_menu_game_do_over, L"待った");
            AppendMenuW(main_menu_game, MF_STRING, main_menu_game_resign, L"投了");

            AppendMenuW(main_menu_help, MF_STRING, main_menu_help_version, L"バージョン情報");
        }
    }

    LRESULT CALLBACK window_main::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
            case WM_TIMER: {
                if (w_param == timer_id) {
                    if (!ctrl_down) return 0;
                    if (key_held['H']) grid_scroll(-scroll_speed, 0);
                    if (key_held['L']) grid_scroll(scroll_speed, 0);
                    if (key_held['J']) grid_scroll(0, scroll_speed);
                    if (key_held['K']) grid_scroll(0, -scroll_speed);
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                return 0;
            }
            case WM_CLOSE: {
                if (!kifu_saved) {
                    int ret = check_nosave();
                    switch (ret) {
                        case 0: {
                            SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OVERWRITE_SAVE, 0), 0);
                            break;
                        }
                        case 1: {
                            break;
                        }
                        case 2: {
                            return 0;
                        }
                    }
                }
                save_config();
                handle_exit();
                return 0;
            }
            case WM_KEYDOWN: {
                int virtual_key = static_cast<int>(w_param);
                key_held[virtual_key] = true;
                if (virtual_key == VK_CONTROL) ctrl_down = true;
                return 0;
            }
            case WM_KEYUP: {
                int virtual_key = static_cast<int>(w_param);
                key_held[virtual_key] = false;
                if (virtual_key == VK_CONTROL) ctrl_down = false;
                return 0;
            }
            case WM_SYSKEYDOWN: {
                switch (w_param) {
                    case 'F':
                    case 'E':
                    case 'V':
                    case 'H':
                    case 'N':
                    case 'O':
                    case 'S':
                    case 'A': {
                        SendMessageW(handle_window, WM_SYSCOMMAND, SC_KEYMENU, w_param);
                    }
                }
                return 0;
            }
            case WM_DESTROY: {
                KillTimer(handle_window, timer_id);
                newgame_window.release();
                handle_exit();
                return 0;
            }
            case WM_COMMAND: {
                switch (LOWORD(w_param)) {
                    case ID_MENU_FILE_CREATE_NEW: {
                        break;
                    }
                    case ID_MENU_FILE_OPEN: {
                        if (!kifu_saved) {
                            int ret = check_nosave();
                            switch (ret) {
                                case 0: {
                                    SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OVERWRITE_SAVE, 0), 0);
                                    break;
                                }
                                case 1: {
                                    break;
                                }
                                case 2: {
                                    return 0;
                                }
                            }
                        }
                        std::wstring result;
                        show_file_load_dialog(result);
                        current_kifu_path = result;
                        std::string filepath = utf16_to_utf8(result);
                        if (filepath.empty()) {
                            break;
                        }
                        try {
                            current_kifu.kifu_load(filepath);
                        }
                        catch (const std::exception &e) {
                            MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(), L"Error", MB_OK | MB_ICONERROR);
                        }
                        kifu_current_turn = current_kifu.size() - 1;
                        kifu_saved = true;
                        hm::kifuver1_to_board(current_kifu, board, current_kifu.size() - 1);
                        update_title();
                        initialize_scroll();
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                    }
                    case ID_MENU_FILE_OVERWRITE_SAVE: {
                        if (current_kifu_path.empty()) {
                            SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_SAVE_AS, 0), 0);
                        }
                        else {
                            try {
                                current_kifu.kifu_save(utf16_to_utf8(current_kifu_path));
                                kifu_saved = true;
                            }
                            catch (const std::exception &e) {
                                MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(),
                                            L"Error", MB_OK | MB_ICONERROR);
                            }
                        }
                        break;
                    }
                    case ID_MENU_FILE_SAVE_AS: {
                        std::wstring result;
                        show_file_save_dialog(result);
                        current_kifu_path = result;
                        std::string filepath = utf16_to_utf8(result);
                        if (filepath.empty()) {
                            break;
                        }
                        try {
                            current_kifu.kifu_save(filepath);
                            kifu_saved = true;
                        }
                        catch (const std::exception &e) {
                            MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(),
                                        L"Error", MB_OK | MB_ICONERROR);
                        }
                        update_title();
                        break;
                    }
                    case ID_MENU_FILE_CLOSE: {
                        if (!kifu_saved) {
                            int ret = check_nosave();
                            switch (ret) {
                                case 0: {
                                    SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OVERWRITE_SAVE, 0), 0);
                                    break;
                                }
                                case 1: {
                                    break;
                                }
                                case 2: {
                                    return 0;
                                }
                            }
                        }
                        current_kifu_path.clear();
                        current_kifu.clear();
                        kifu_current_turn = 0;
                        board.clear();
                        kifu_saved = true;
                        update_title();
                        initialize_scroll();
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                    }
                    case ID_MENU_FILE_SETTINGS: {
                        settings_window.show_window(SW_SHOW, window_area_rectf.left + 30.0f, window_area_rectf.top + 30.0f);
                        break;
                    }
                    case ID_MENU_FILE_EXIT: {
                        if (!kifu_saved) {
                            int ret = check_nosave();
                            switch (ret) {
                                case 0: {
                                    SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OVERWRITE_SAVE, 0), 0);
                                    break;
                                }
                                case 1: {
                                    break;
                                }
                                case 2: {
                                    return 0;
                                }
                            }
                        }
                        handle_exit();
                        break;
                    }
                    case ID_MENU_EDIT_MOVE_FORWARD: {
                        if (current_kifu.size() && kifu_current_turn < static_cast<int>(current_kifu.size() - 1)) {
                            kifu_current_turn++;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn);
                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_STEP_BACK: {
                        if (current_kifu.size() && kifu_current_turn > 0) {
                            kifu_current_turn--;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn);
                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_JUMP_TO_FIRST_MOVE: {
                        if (current_kifu.size()) {
                            kifu_current_turn = 0;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn);
                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_COMMENT: {
                        break;
                    }
                    case ID_MENU_EDIT_KIFU_INFO: {
                        break;
                    }
                    case ID_MENU_VIEW_BOARD_SCROLL_RESET: {
                        set_grid_scroll(-(config_ref.grid_size_x - config_ref.margin * 2) / 2 + config_ref.grid_spacing / 2, (config_ref.grid_and_kifu_size_y - config_ref.margin * 2) / 2 - config_ref.grid_spacing / 2);
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                    }
                    case ID_MENU_VIEW_BOARD_SEPARATE_WINDOW: {
                        break;
                    }
                    case ID_MENU_GAME_NEW: {
                        newgame_window.show_window(SW_SHOW, window_area_rectf.left + 30.0f, window_area_rectf.top + 30.0f);
                        break;
                    }
                    case ID_MENU_GAME_DO_OVER: {
                        if (!is_gaming) return 0;
                        if (kifu_current_turn > -1) {
                            kifu_current_turn = current_kifu.size() - 2;
                            current_kifu.remove();
                            if (current_kifu.size() == 0) {
                                board.clear();
                            } else {
                                hm::kifuver1_to_board(current_kifu, board, kifu_current_turn);
                            }
                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        return 0;
                    }
                    case ID_MENU_GAME_RESIGN: {
                        if (!is_gaming) return 0;
                        is_gaming = false;
                        InvalidateRect(handle_window, nullptr, FALSE);
                        MessageBoxW(NULL, std::wstring(L"まで" + std::to_wstring(current_kifu.size()) + L"手で" + (((current_kifu.size() - 1) % 2 == 0) ? L"先手" : L"後手") + L"の勝ち").c_str(), L"", MB_OK);
                        break;
                    }
                    case ID_MENU_HELP_VERSION: {
                        version_window.show_window(SW_SHOW, window_area_rectf.left + 30.0f, window_area_rectf.top + 30.0f);
                        break;
                    }
                }
                return 0;
            }
            case WM_MOUSEWHEEL: {
                POINT point = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };
                ScreenToClient(handle_window, &point);

                if (PtInRect(&grid_area_rect, point)) {
                    short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                    if (ctrl_down) {
                        grid_scroll((float)-delta / 8, 0);
                    }
                    else {
                        grid_scroll(0, (float)-delta / 8);
                    }
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                else if (PtInRect(&kifu_area_rect, point)) {
                    short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                    kifu_scroll(0, (float)-delta / 8);
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                else if (PtInRect(&config_area_rect, point)) {
                    short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                    config_scroll(0, (float)-delta / 8);
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                return 0;
            }
            case WM_MOUSEHWHEEL: {
                POINT point = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };
                ScreenToClient(handle_window, &point);

                if (PtInRect(&grid_area_rect, point)) {
                    short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                    grid_scroll((float)delta / 8, 0);
                    InvalidateRect(handle_window, nullptr, FALSE);
                }

                return 0;
            }
            case WM_LBUTTONDOWN: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

                if (PtInRect(&grid_area_rect, pt) && is_gaming) {
                    float gx = static_cast<float>(pt.x);
                    float gy = static_cast<float>(pt.y);
                    float fx = (gx - grid_area_rectf.left + main_window.grid_scroll_offset.x) / config_ref.grid_spacing - 0.5f;
                    float fy = (grid_area_rectf.bottom - main_window.grid_scroll_offset.y - gy) / config_ref.grid_spacing - 0.5f;
                    int x = static_cast<int>(std::round(fx));
                    int y = static_cast<int>(std::round(fy));
                    if (board.has_piece(x, y)) {
                        return 0;
                    }
                    try {
                        main_window.board.progress(x, y);
                        kifu_current_turn = current_kifu.size();
                        current_kifu.add(x, y);
                        hm::kifuver1_to_board(current_kifu, board, kifu_current_turn);
                        kifu_saved = false;
                        InvalidateRect(handle_window, nullptr, FALSE);
                        if (board.is_win()) SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_GAME_RESIGN, 0), 0);
                    } catch (...) {
                        return 0;
                    }
                    return 0;
                }
                if (PtInRect(&do_over_button_area_rect, pt)) {
                    do_over_button_state = 2;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }
                if (PtInRect(&resign_button_area_rect, pt)) {
                    resign_button_state = 2;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }
                if (PtInRect(&kifu_area_rect, pt)) {
                    float relative_y = pt.y - kifu_area_rect.top + kifu_scroll_offset.y;
                    unsigned int clicked_turn = static_cast<unsigned int>(relative_y / config_ref.kifu_spacing);
                    if (clicked_turn < current_kifu.size()) {
                        kifu_current_turn = clicked_turn;
                        hm::kifuver1_to_board(current_kifu, board, clicked_turn);
                        InvalidateRect(handle_window, nullptr, FALSE);
                        return 0;
                    }
                }

                const int tol = 5;
                float boundary1 = config_ref.grid_size_x;
                float boundary2 = config_ref.grid_size_x + config_ref.kifu_size_x;

                if (std::abs(pt.x - boundary1) <= tol && pt.y > config_ref.margin && pt.y < config_ref.grid_and_kifu_size_y - config_ref.margin) {
                    is_resizing = true;
                    cr_resize_region = resize_region::grid_kifu;
                    resize_start = pt;
                    initial_grid_size = config_ref.grid_size_x;
                    initial_kifu_size = config_ref.kifu_size_x;
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                    SetCapture(handle_window);
                    return 0;
                }
                else if (std::abs(pt.x - boundary2) <= tol && pt.y > config_ref.margin && pt.y < config_ref.grid_and_kifu_size_y - config_ref.margin) {
                    is_resizing = true;
                    cr_resize_region = resize_region::kifu_config;
                    resize_start = pt;
                    initial_kifu_size = config_ref.kifu_size_x;
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                    SetCapture(handle_window);
                    return 0;
                }
                else if (std::abs(pt.y - config_ref.grid_and_kifu_size_y) <= tol) {
                    is_resizing = true;
                    cr_resize_region = resize_region::vertical;
                    resize_start = pt;
                    initial_grid_and_kifu_size_y = config_ref.grid_and_kifu_size_y;
                    SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                    SetCapture(handle_window);
                    return 0;
                } else {
                    is_resizing = false;
                    cr_resize_region = resize_region::none;
                }

                InvalidateRect(handle_window, nullptr, FALSE);
                return 0;
            }
            case WM_LBUTTONUP: {
                if (is_resizing) {
                    is_resizing = false;
                    cr_resize_region = resize_region::none;
                    ReleaseCapture();
                    return 0;
                }

                if (do_over_button_state == 2) {
                    if (!is_gaming) return 0;
                    if (kifu_current_turn > -1) {
                        kifu_current_turn = current_kifu.size() - 2;
                        current_kifu.remove();
                        if (current_kifu.size() == 0) {
                            board.clear();
                        } else {
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn);
                        }
                        InvalidateRect(handle_window, nullptr, FALSE);
                    }
                    do_over_button_state = 1;
                    return 0;
                }
                if (resign_button_state == 2) {
                    if (!is_gaming) return 0;
                    is_gaming = false;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    MessageBoxW(NULL, std::wstring(L"まで" + std::to_wstring(current_kifu.size()) + L"手で" + (((current_kifu.size() - 1) % 2 == 0) ? L"先手" : L"後手") + L"の勝ち").c_str(), L"", MB_OK);
                    resign_button_state = 1;
                }

                break;
            }
            case WM_MOUSEMOVE: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };
                if (is_resizing) {
                    switch (cr_resize_region) {
                        case resize_region::grid_kifu: {
                            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                            int dx = pt.x - resize_start.x;
                            config_ref.grid_size_x = initial_grid_size + dx;
                            config_ref.kifu_size_x = initial_kifu_size - dx;
                            break;
                        }
                        case resize_region::kifu_config: {
                            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                            int dx = pt.x - resize_start.x;
                            config_ref.kifu_size_x = initial_kifu_size + dx;
                            break;
                        }
                        case resize_region::vertical: {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                            int dy = pt.y - resize_start.y;
                            config_ref.grid_and_kifu_size_y = initial_grid_and_kifu_size_y + dy;
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    update_rect();
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }

                if (PtInRect(&do_over_button_area_rect, pt)) {
                    do_over_button_state = 1;
                }
                else {
                    do_over_button_state = 0;
                }
                if (PtInRect(&resign_button_area_rect, pt)) {
                    resign_button_state = 1;
                }
                else {
                    resign_button_state = 0;
                }

                const int tol = 5;
                float boundary1 = config_ref.grid_size_x;
                float boundary2 = config_ref.grid_size_x + config_ref.kifu_size_x;
                if ((std::abs(pt.x - boundary1) <= tol || std::abs(pt.x - boundary2) <= tol) && config_ref.margin < static_cast<float>(pt.y) && config_ref.grid_and_kifu_size_y - config_ref.margin > static_cast<float>(pt.y)) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                }
                else if (std::abs(pt.y - config_ref.grid_and_kifu_size_y) <= tol) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                }
                else {
                    SetCursor(LoadCursor(nullptr, IDC_ARROW));
                }

                InvalidateRect(handle_window, nullptr, FALSE);
                return 0;
            }
            case WM_PAINT: {
                PAINTSTRUCT paint_struct;
                HDC handle_device_context = BeginPaint(handle_window, &paint_struct);
                d2d1_render_target->BeginDraw();
                redraw();
                d2d1_render_target->EndDraw();
                EndPaint(handle_window, &paint_struct);
                return 0;
            }
            case WM_DPICHANGED: {
                UINT dpi_x = HIWORD(w_param);
                UINT dpi_y = LOWORD(w_param);

                RECT* suggested_rect = (RECT*)l_param;
                SetWindowPos(handle_window,
                            NULL,
                            suggested_rect->left,
                            suggested_rect->top,
                            suggested_rect->right - suggested_rect->left,
                            suggested_rect->bottom - suggested_rect->top,
                            SWP_NOZORDER | SWP_NOACTIVATE);

                if (d2d1_render_target) {
                    d2d1_render_target->SetDpi((FLOAT)dpi_x, (FLOAT)dpi_y);
                }

                InvalidateRect(handle_window, NULL, TRUE);
                return 0;
            }
            case WM_MOVE: {
                grobal_config.window_pos_x = static_cast<float>(LOWORD(l_param));
                grobal_config.window_pos_y = static_cast<float>(HIWORD(l_param));
                update_rect();
                return 0;
            }
            case WM_SIZE: {
                grobal_config.window_size_x = static_cast<float>(LOWORD(l_param));
                grobal_config.window_size_y = static_cast<float>(HIWORD(l_param));
                update_rect();
                UINT width = LOWORD(l_param);
                UINT height = HIWORD(l_param);
                if (d2d1_render_target) {
                    D2D1_SIZE_U size = D2D1::SizeU(width, height);
                    d2d1_render_target->Resize(size);
                }
                InvalidateRect(handle_window, nullptr, FALSE);
                return 0;
            }
            default: {
                return DefWindowProcW(handle_window, message, w_param, l_param);
            }
        }
        return DefWindowProcW(handle_window, message, w_param, l_param);
    }

    LRESULT CALLBACK window_newgame::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
            case WM_COMMAND: {
                switch (LOWORD(w_param)) {
                    case ID_NEWGAME_BUTTON: {
                        if (!main_window.kifu_saved) {
                            int ret = check_nosave();
                            switch (ret) {
                                case 0: {
                                    SendMessageW(main_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OVERWRITE_SAVE, 0), 0);
                                }
                                case 1: {
                                    break;
                                }
                                case 2: {
                                    return 0;
                                }
                            }
                        }
                        main_window.board.clear();
                        main_window.current_kifu.clear();
                        main_window.kifu_current_turn = 0;
                        main_window.is_gaming = true;
                        main_window.kifu_saved = false;
                        main_window.current_kifu_path.clear();
                        main_window.initialize_scroll();
                        update_title();
                        InvalidateRect(main_window, nullptr, FALSE);
                        handle_exit();
                        break;
                    }
                }
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC handle_device_context = BeginPaint(handle_window, &ps);
                d2d1_render_target->BeginDraw();
                redraw();
                d2d1_render_target->EndDraw();
                EndPaint(handle_window, &ps);
                return 0;
            }
            case WM_CLOSE: {
                handle_exit();
                return 0;
            }
            default: {
                return DefWindowProcW(handle_window, message, w_param, l_param);
            }
        }
        return DefWindowProcW(handle_window, message, w_param, l_param);
    }

    LRESULT CALLBACK window_settings::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC handle_device_context = BeginPaint(handle_window, &ps);
                d2d1_render_target->BeginDraw();
                redraw();
                d2d1_render_target->EndDraw();
                EndPaint(handle_window, &ps);
                return 0;
            }
            case WM_CLOSE: {
                handle_exit();
                main_window.d2d1_initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory);
                main_window.update_rect();
                return 0;
            }
            case WM_DPICHANGED: {
                UINT dpi_x = HIWORD(w_param);
                UINT dpi_y = LOWORD(w_param);

                RECT* suggested_rect = (RECT*)l_param;
                SetWindowPos(handle_window,
                            NULL,
                            suggested_rect->left,
                            suggested_rect->top,
                            suggested_rect->right - suggested_rect->left,
                            suggested_rect->bottom - suggested_rect->top,
                            SWP_NOZORDER | SWP_NOACTIVATE);

                if (d2d1_render_target) {
                    d2d1_render_target->SetDpi((FLOAT)dpi_x, (FLOAT)dpi_y);
                }

                InvalidateRect(handle_window, NULL, TRUE);
                return 0;
            }
            case WM_MOVE: {
                update_rect();
                return 0;
            }
            case WM_SIZE: {
                update_rect();
                UINT width = LOWORD(l_param);
                UINT height = HIWORD(l_param);
                if (d2d1_render_target)
                {
                    D2D1_SIZE_U size = D2D1::SizeU(width, height);
                    d2d1_render_target->Resize(size);
                }
                InvalidateRect(handle_window, nullptr, FALSE);
                return 0;
            }
            default: {
                return DefWindowProcW(handle_window, message, w_param, l_param);
            }
        }
        return DefWindowProcW(handle_window, message, w_param, l_param);
    }

    LRESULT CALLBACK window_version::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC handle_device_context = BeginPaint(handle_window, &ps);
                d2d1_render_target->BeginDraw();
                redraw();
                d2d1_render_target->EndDraw();
                EndPaint(handle_window, &ps);
                return 0;
            }
            case WM_CLOSE: {
                handle_exit();
                main_window.d2d1_initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory);
                main_window.update_rect();
                return 0;
            }
            case WM_DPICHANGED: {
                UINT dpi_x = HIWORD(w_param);
                UINT dpi_y = LOWORD(w_param);

                RECT* suggested_rect = (RECT*)l_param;
                SetWindowPos(handle_window,
                            NULL,
                            suggested_rect->left,
                            suggested_rect->top,
                            suggested_rect->right - suggested_rect->left,
                            suggested_rect->bottom - suggested_rect->top,
                            SWP_NOZORDER | SWP_NOACTIVATE);

                if (d2d1_render_target) {
                    d2d1_render_target->SetDpi((FLOAT)dpi_x, (FLOAT)dpi_y);
                }

                InvalidateRect(handle_window, NULL, TRUE);
                return 0;
            }
            case WM_SIZE: {
                UINT width = LOWORD(l_param);
                UINT height = HIWORD(l_param);
                if (d2d1_render_target)
                {
                    D2D1_SIZE_U size = D2D1::SizeU(width, height);
                    d2d1_render_target->Resize(size);
                }
                InvalidateRect(handle_window, nullptr, FALSE);
                return 0;
            }
            default: {
                return DefWindowProcW(handle_window, message, w_param, l_param);
            }
        }
        return DefWindowProcW(handle_window, message, w_param, l_param);
    }
}

int WINAPI wWinMain(HINSTANCE handle_instance, HINSTANCE, LPWSTR, int) {
    hmgui::load_config();
    main_menu.create_menu();
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &grobal_d2d1_factory);
    if (FAILED(hr)) return false;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&grobal_d2d1_dwrite_factory));
    if (FAILED(hr)) return false;

    main_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory);
    newgame_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory, main_window);
    settings_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory, main_window);
    version_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory, main_window);
    main_window.show_window();

    SetMenu(main_window, main_menu);

    timer_id = SetTimer(main_window, timer_id, 16, nullptr);

    MSG message;
    while (true) {
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                return static_cast<int>(message.wParam);
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        Sleep(10);
    }
}
