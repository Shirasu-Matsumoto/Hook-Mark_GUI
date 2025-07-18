﻿#include <hookmark_gui.hpp>

hmgui::window_conf grobal_config;

hmgui::window_main main_window(grobal_config);
hmgui::wc_main main_window_class;
hmgui::window_newgame newgame_window(grobal_config);
hmgui::wc_newgame newgame_window_class;
hmgui::window_settings settings_window(grobal_config);
hmgui::wc_settings settings_window_class;
hmgui::window_version version_window;
hmgui::wc_version version_window_class;
hmgui::window_sep_board sep_board_window(grobal_config);
hmgui::wc_sep_board sep_board_window_class;
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
                        main_menu_file_save_config(ID_MENU_FILE_SAVE_CONFIG),
                        main_menu_file_clear_config(ID_MENU_FILE_CLEAR_CONFIG),
                        main_menu_file_exit(ID_MENU_FILE_EXIT),
                        main_menu_file_nosave_config_exit(ID_MENU_FILE_NOSAVE_CONFIG_EXIT),
                        main_menu_edit_move_forward(ID_MENU_EDIT_MOVE_FORWARD),
                        main_menu_edit_step_back(ID_MENU_EDIT_STEP_BACK),
                        main_menu_edit_jump_to_initial_phase(ID_MENU_EDIT_JUMP_TO_INITIAL_PHASE),
                        main_menu_edit_jump_to_last_phase(ID_MENU_EDIT_JUMP_TO_LAST_PHASE),
                        main_menu_edit_board(ID_MENU_EDIT_BOARD),
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

bool key_held[256] = {};
UINT_PTR main_timer_id = 1;
UINT_PTR sep_board_timer_id = 2;

namespace hmgui {
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
        GetEnvironmentVariableW(L"APPDATA", path, MAX_PATH);
        std::filesystem::path dir = std::filesystem::path(utf16_to_utf8(path)) / L"Hook-Mark_GUI";
        if (!std::filesystem::exists(dir / "config")) {
            std::filesystem::create_directories(dir / "config");
        }
        std::string init_path = (dir / "config" / "init.cfg").string();

        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::filesystem::path exe_path(utf16_to_utf8(path));
        dir = exe_path.parent_path();
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
        if (um_config.count("window_state")) grobal_config.window_state = to_float(um_config.at("window_state"));
        if (um_config.count("margin")) grobal_config.margin = to_float(um_config.at("margin"));
        if (um_config.count("padding")) grobal_config.padding = to_float(um_config.at("padding"));
        if (um_config.count("window_size_x")) grobal_config.window_size_x = to_float(um_config.at("window_size_x"));
        if (um_config.count("window_size_y")) grobal_config.window_size_y = to_float(um_config.at("window_size_y"));
        if (um_config.count("grid_spacing")) grobal_config.grid_spacing = to_float(um_config.at("grid_spacing"));
        if (um_config.count("grid_size_x")) grobal_config.grid_size_x = to_float(um_config.at("grid_size_x"));
        if (um_config.count("kifu_size_x")) grobal_config.kifu_size_x = to_float(um_config.at("kifu_size_x"));
        if (um_config.count("kifu_turn_size_x")) grobal_config.kifu_turn_size_x = to_float(um_config.at("kifu_turn_size_x"));
        if (um_config.count("vertical_size")) grobal_config.vertical_size = to_float(um_config.at("vertical_size"));
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
        GetEnvironmentVariableW(L"APPDATA", path, MAX_PATH);
        std::filesystem::path dir = std::filesystem::path(utf16_to_utf8(path)) / L"Hook-Mark_GUI";
        if (!std::filesystem::exists(dir / "config")) {
            std::filesystem::create_directories(dir / "config");
        }
        std::ofstream file(dir / "config" / "init.cfg");

        if (!file.is_open()) return;

        file << "[Hook-Mark GUI Window Initialize Config]\n\n# Main window\n";
        file << "window_pos_x = " << grobal_config.window_pos_x << "\n";
        file << "window_pos_y = " << grobal_config.window_pos_y << "\n";
        file << "window_state = " << grobal_config.window_state << "\n";
        file << "margin = " << grobal_config.margin << "\n";
        file << "padding = " << grobal_config.padding << "\n";
        file << "window_size_x = " << grobal_config.window_size_x << "\n";
        file << "window_size_y = " << grobal_config.window_size_y << "\n";
        file << "grid_spacing = " << grobal_config.grid_spacing << "\n";
        file << "kifu_spacing = " << grobal_config.kifu_spacing << "\n";
        file << "grid_size_x = " << grobal_config.grid_size_x << "\n";
        file << "kifu_size_x = " << grobal_config.kifu_size_x << "\n";
        file << "kifu_turn_size_x = " << grobal_config.kifu_turn_size_x << "\n";
        file << "vertical_size = " << grobal_config.vertical_size << "\n";
        file << "open_file = \"" << grobal_config.open_file << "\"\n";
        file << "label_size = " << grobal_config.label_size << "\n\n# New game window\n";
        file << "first_name = \"" << grobal_config.first_name << "\"\n";
        file << "second_name = \"" << grobal_config.second_name << "\"\n";
        file << "first_time = " << grobal_config.first_time << "\n";
        file << "second_time = " << grobal_config.second_time << "\n";
        file << "first_countdown = " << grobal_config.first_countdown << "\n";
        file << "second_countdown = " << grobal_config.second_countdown << "\n";
        file << "lose_time_runs_out = " << (grobal_config.lose_time_runs_out ? "true" : "false") << "\n";

        file.close();
    }

    void clear_config() {
        wchar_t path[MAX_PATH];
        GetEnvironmentVariableW(L"APPDATA", path, MAX_PATH);
        std::filesystem::path dir = std::filesystem::path(utf16_to_utf8(path)) / L"Hook-Mark_GUI";
        if (!std::filesystem::exists(dir / "config")) {
            std::filesystem::create_directories(dir / "config");
        }
        std::filesystem::path config_path = dir / "config" / "init.cfg";
        try {
            if (std::filesystem::exists(config_path)) {
                std::filesystem::remove(config_path);
            }
        }
        catch (std::exception &e) {
            MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(), L"エラー", MB_OK | MB_ICONERROR);
        }
        grobal_config = window_conf();
        main_window.d2d1_update_text_format();
    }

    void init_open_file() {
        hm::kifu_ver1 temp;
        try {
            temp.kifu_load(grobal_config.open_file);
        }
        catch (...) {
            return;
        }
        main_window.current_kifu = temp;
        main_window.current_kifu_path = grobal_config.open_file;
        main_window.current_kifu.resign();
        main_window.kifu_current_turn = main_window.current_kifu.size() - 1;
        main_window.kifu_saved = true;
        main_window.update_title();
        hm::kifuver1_to_board(main_window.current_kifu, main_window.board);
        float visible_top = main_window.kifu_scroll_offset.y;
        float visible_bottom = main_window.kifu_scroll_offset.y + (main_window.kifu_area_rect.bottom - main_window.kifu_area_rect.top);
        float turn_pos = (main_window.kifu_current_turn + 1) * grobal_config.kifu_spacing;

        if (turn_pos < visible_top) {
            main_window.kifu_scroll_offset.y = turn_pos;
        } else if (turn_pos > visible_bottom - grobal_config.kifu_spacing) {
            main_window.kifu_scroll_offset.y = turn_pos - (main_window.kifu_area_rect.bottom - main_window.kifu_area_rect.top) + grobal_config.kifu_spacing;
        }

        if (main_window.kifu_scroll_offset.y < 0.0f) main_window.kifu_scroll_offset.y = 0.0f;
        main_window.initialize_scroll();
        InvalidateRect(main_window, nullptr, FALSE);
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
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_save_config, L"設定を保存");
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_clear_config, L"設定をクリア");
            AppendMenuW(main_menu_file, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_exit, L"終了");
            AppendMenuW(main_menu_file, MF_STRING, main_menu_file_nosave_config_exit, L"設定を保存せずに終了");

            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_step_back, L"一手戻る");
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_move_forward, L"一手進む");
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_jump_to_initial_phase, L"初期局面に戻る");
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_jump_to_last_phase, L"最終局面に進む");
            AppendMenuW(main_menu_edit, MF_SEPARATOR, 0, NULL);
            AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_board, L"局面編集を開始");
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
                if (w_param == main_timer_id) {
                    if (GetForegroundWindow() != handle_window) return 0;

                    if (key_held[VK_UP] && current_focus == focus::grid) grid_scroll(0, -scroll_speed);
                    if (key_held[VK_DOWN] && current_focus == focus::grid) grid_scroll(0, scroll_speed);
                    if (key_held[VK_LEFT] && current_focus == focus::grid) grid_scroll(-scroll_speed, 0);
                    if (key_held[VK_RIGHT] && current_focus == focus::grid) grid_scroll(scroll_speed, 0);

                    if (key_held[VK_UP] && current_focus == focus::config) config_scroll(0, -scroll_speed);
                    if (key_held[VK_DOWN] && current_focus == focus::config) config_scroll(0, scroll_speed);

                    if (key_held[VK_CONTROL] && key_held['H']) grid_scroll(-scroll_speed, 0);
                    if (key_held[VK_CONTROL] && key_held['L']) grid_scroll(scroll_speed, 0);
                    if (key_held[VK_CONTROL] && key_held['J']) grid_scroll(0, scroll_speed);
                    if (key_held[VK_CONTROL] && key_held['K']) grid_scroll(0, -scroll_speed);

                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                return 0;
            }
            case WM_CLOSE: {
                if (!kifu_saved) {
                    int ret = check_nosave(handle_window);
                    switch (ret) {
                        case 0: {
                            if (overwrite_save_handler()) {
                                return 0;
                            }
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
                if (!key_held[VK_CONTROL]) {
                    save_config();
                }
                handle_exit();
                return 0;
            }
            case WM_KEYDOWN: {
                int virtual_key = static_cast<int>(w_param);
                key_held[virtual_key] = true;

                if (key_held[VK_CONTROL] && virtual_key == 'S') SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OVERWRITE_SAVE, 0), 0);
                if (key_held[VK_CONTROL] && virtual_key == 'O') SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_OPEN, 0), 0);
                if (key_held[VK_CONTROL] && virtual_key == 'N') SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_FILE_CREATE_NEW, 0), 0);
                if (current_focus == focus::kifu && virtual_key == VK_UP) SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_STEP_BACK, 0), 0);
                if (current_focus == focus::kifu && virtual_key == VK_DOWN) SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_MOVE_FORWARD, 0), 0);
                if (current_focus == focus::kifu && key_held[VK_CONTROL] && virtual_key == VK_UP) SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_JUMP_TO_INITIAL_PHASE, 0), 0);
                if (current_focus == focus::kifu && key_held[VK_CONTROL] && virtual_key == VK_DOWN) SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_JUMP_TO_LAST_PHASE, 0), 0);

                return 0;
            }
            case WM_KEYUP: {
                int virtual_key = static_cast<int>(w_param);
                key_held[virtual_key] = false;
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
                KillTimer(handle_window, main_timer_id);
                newgame_window.release();
                settings_window.release();
                sep_board_window.release();
                version_window.release();
                handle_exit();
                return 0;
            }
            case WM_COMMAND: {
                switch (LOWORD(w_param)) {
                    case ID_MENU_FILE_CREATE_NEW: {
                        if (!kifu_saved) {
                            int ret = check_nosave(handle_window);
                            switch (ret) {
                                case 0: {
                                    if (overwrite_save_handler()) {
                                        return 0;
                                    }
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
                        kifu_current_turn = -1;
                        board.clear();
                        is_gaming = false;
                        if (is_editing) {
                            SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_BOARD, NULL), MAKELPARAM(NULL, NULL));
                        }
                        EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_GRAYED);
                        EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_GRAYED);
                        do_over_button.enabled = false;
                        resign_button.enabled = false;
                        DrawMenuBar(main_window);
                        kifu_saved = true;
                        config_ref.open_file.clear();
                        update_title();
                        initialize_scroll();
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                        break;
                    }
                    case ID_MENU_FILE_OPEN: {
                        if (!kifu_saved) {
                            int ret = check_nosave(handle_window);
                            switch (ret) {
                                case 0: {
                                    if (overwrite_save_handler()) {
                                        return 0;
                                    }
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
                        if (result.empty()) {
                            break;
                        }
                        if (is_editing) {
                            SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_BOARD, NULL), MAKELPARAM(NULL, NULL));
                        }
                        hm::kifu_ver1 temp;
                        try {
                            temp.kifu_load(result);
                        }
                        catch (const std::exception &e) {
                            MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(), L"エラー", MB_OK | MB_ICONERROR);
                            break;
                        }
                        current_kifu = temp;
                        current_kifu_path = result;
                        current_kifu.resign();
                        kifu_current_turn = current_kifu.size() - 1;
                        kifu_saved = true;
                        config_ref.open_file = utf16_to_utf8(result);
                        hm::kifuver1_to_board(current_kifu, board);
                        float visible_top = kifu_scroll_offset.y;
                        float visible_bottom = kifu_scroll_offset.y + (kifu_area_rect.bottom - kifu_area_rect.top);
                        float turn_pos = (kifu_current_turn + 1) * config_ref.kifu_spacing;

                        if (turn_pos < visible_top) {
                            kifu_scroll_offset.y = turn_pos;
                        } else if (turn_pos > visible_bottom - config_ref.kifu_spacing) {
                            kifu_scroll_offset.y = turn_pos - (kifu_area_rect.bottom - kifu_area_rect.top) + config_ref.kifu_spacing;
                        }

                        if (kifu_scroll_offset.y < 0.0f) kifu_scroll_offset.y = 0.0f;
                        update_title();
                        initialize_scroll();
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                    }
                    case ID_MENU_FILE_OVERWRITE_SAVE: {
                        overwrite_save_handler();
                        break;
                    }
                    case ID_MENU_FILE_SAVE_AS: {
                        save_as_handler();
                        break;
                    }
                    case ID_MENU_FILE_CLOSE: {
                        if (!kifu_saved) {
                            int ret = check_nosave(handle_window);
                            switch (ret) {
                                case 0: {
                                    if (overwrite_save_handler()) {
                                        return 0;
                                    }
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
                        kifu_current_turn = -1;
                        board.clear();
                        is_gaming = false;
                        if (is_editing) {
                            SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_BOARD, NULL), MAKELPARAM(NULL, NULL));
                        }
                        EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_GRAYED);
                        EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_GRAYED);
                        do_over_button.enabled = false;
                        resign_button.enabled = false;
                        DrawMenuBar(main_window);
                        kifu_saved = true;
                        config_ref.open_file.clear();
                        update_title();
                        initialize_scroll();
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                    }
                    case ID_MENU_FILE_SETTINGS: {
                        settings_window.show_window(SW_SHOW, window_area_rectf.left + 30.0f, window_area_rectf.top + 30.0f);
                        break;
                    }
                    case ID_MENU_FILE_SAVE_CONFIG: {
                        save_config();
                        break;
                    }
                    case ID_MENU_FILE_CLEAR_CONFIG: {
                        clear_config();
                        d2d1_update_text_format();
                        break;
                    }
                    case ID_MENU_FILE_EXIT: {
                        if (!kifu_saved) {
                            int ret = check_nosave(handle_window);
                            switch (ret) {
                                case 0: {
                                    if (overwrite_save_handler()) {
                                        return 0;
                                    }
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
                        break;
                    }
                    case ID_MENU_FILE_NOSAVE_CONFIG_EXIT: {
                        if (!kifu_saved) {
                            int ret = check_nosave(handle_window);
                            switch (ret) {
                                case 0: {
                                    if (overwrite_save_handler()) {
                                        return 0;
                                    }
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
                    case ID_MENU_EDIT_STEP_BACK: {
                        if (current_kifu.size() && kifu_current_turn > -1) {
                            kifu_current_turn--;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);

                            float visible_top = kifu_scroll_offset.y;
                            float visible_bottom = kifu_scroll_offset.y + (kifu_area_rect.bottom - kifu_area_rect.top);
                            float turn_pos = (kifu_current_turn + 1) * config_ref.kifu_spacing;

                            if (turn_pos < visible_top) {
                                kifu_scroll_offset.y = turn_pos;
                            } else if (turn_pos > visible_bottom - config_ref.kifu_spacing) {
                                kifu_scroll_offset.y = turn_pos - (kifu_area_rect.bottom - kifu_area_rect.top) + config_ref.kifu_spacing;
                            }

                            if (kifu_scroll_offset.y < 0.0f) kifu_scroll_offset.y = 0.0f;

                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_MOVE_FORWARD: {
                        if (current_kifu.size() && kifu_current_turn < static_cast<int>(current_kifu.size() - 1)) {
                            kifu_current_turn++;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);

                            float visible_top = kifu_scroll_offset.y;
                            float visible_bottom = kifu_scroll_offset.y + (kifu_area_rect.bottom - kifu_area_rect.top);
                            float turn_pos = (kifu_current_turn + 1) * config_ref.kifu_spacing;

                            if (turn_pos < visible_top) {
                                kifu_scroll_offset.y = turn_pos;
                            } else if (turn_pos > visible_bottom - config_ref.kifu_spacing) {
                                kifu_scroll_offset.y = turn_pos - (kifu_area_rect.bottom - kifu_area_rect.top) + config_ref.kifu_spacing;
                            }

                            if (kifu_scroll_offset.y < 0.0f) kifu_scroll_offset.y = 0.0f;

                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_JUMP_TO_INITIAL_PHASE: {
                        if (current_kifu.size()) {
                            kifu_current_turn = -1;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);

                            float visible_top = kifu_scroll_offset.y;
                            float visible_bottom = kifu_scroll_offset.y + (kifu_area_rect.bottom - kifu_area_rect.top);
                            float turn_pos = (kifu_current_turn + 1) * config_ref.kifu_spacing;

                            if (turn_pos < visible_top) {
                                kifu_scroll_offset.y = turn_pos;
                            } else if (turn_pos > visible_bottom - config_ref.kifu_spacing) {
                                kifu_scroll_offset.y = turn_pos - (kifu_area_rect.bottom - kifu_area_rect.top) + config_ref.kifu_spacing;
                            }

                            if (kifu_scroll_offset.y < 0.0f) kifu_scroll_offset.y = 0.0f;

                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_JUMP_TO_LAST_PHASE: {
                        if (current_kifu.size()) {
                            kifu_current_turn = current_kifu.size() - 1;
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);

                            float visible_top = kifu_scroll_offset.y;
                            float visible_bottom = kifu_scroll_offset.y + (kifu_area_rect.bottom - kifu_area_rect.top);
                            float turn_pos = (kifu_current_turn + 1) * config_ref.kifu_spacing;

                            if (turn_pos < visible_top) {
                                kifu_scroll_offset.y = turn_pos;
                            } else if (turn_pos > visible_bottom - config_ref.kifu_spacing) {
                                kifu_scroll_offset.y = turn_pos - (kifu_area_rect.bottom - kifu_area_rect.top) + config_ref.kifu_spacing;
                            }

                            if (kifu_scroll_offset.y < 0.0f) kifu_scroll_offset.y = 0.0f;

                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        break;
                    }
                    case ID_MENU_EDIT_BOARD: {
                        is_editing = !is_editing;
                        if (is_editing) {
                            if (!kifu_saved) {
                                int ret = check_nosave(handle_window);
                                switch (ret) {
                                    case 0: {
                                        if (overwrite_save_handler()) {
                                            return 0;
                                        }
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
                            is_gaming = false;
                            EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_GRAYED);
                            EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_GRAYED);
                            do_over_button.enabled = false;
                            resign_button.enabled = false;
                            current_kifu_path.clear();
                            current_kifu.clear();
                            MENUITEMINFOW menu_item_info = {};
                            menu_item_info.cbSize = sizeof(MENUITEMINFOW);
                            menu_item_info.fMask = MIIM_STRING;
                            menu_item_info.dwTypeData = const_cast<LPWSTR>(L"局面編集を終了");
                            menu_item_info.cch = lstrlenW(L"局面編集を終了");
                            SetMenuItemInfoW(main_menu_edit, main_menu_edit_board, FALSE, &menu_item_info);
                            DrawMenuBar(main_window);
                            InvalidateRect(handle_window, nullptr, FALSE);
                        } else {
                            MENUITEMINFOW menu_item_info = {};
                            menu_item_info.cbSize = sizeof(MENUITEMINFOW);
                            menu_item_info.fMask = MIIM_STRING;
                            menu_item_info.dwTypeData = const_cast<LPWSTR>(L"局面編集を開始");
                            menu_item_info.cch = lstrlenW(L"局面編集を開始");
                            SetMenuItemInfoW(main_menu_edit, main_menu_edit_board, FALSE, &menu_item_info);
                            DrawMenuBar(main_window);
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
                        set_grid_scroll(-(config_ref.grid_size_x - config_ref.margin * 2) / 2 + config_ref.grid_spacing / 2, (config_ref.vertical_size - config_ref.margin * 2) / 2 - config_ref.grid_spacing / 2);
                        InvalidateRect(handle_window, nullptr, FALSE);
                        break;
                    }
                    case ID_MENU_VIEW_BOARD_SEPARATE_WINDOW: {
                        sep_board_window.show_window(SW_SHOW, window_area_rectf.left + 30.0f, window_area_rectf.top + 30.0f, board, current_kifu, kifu_current_turn, grid_scroll_offset);
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
                                hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);
                            }
                            InvalidateRect(handle_window, nullptr, FALSE);
                        }
                        DrawMenuBar(main_window);
                        return 0;
                    }
                    case ID_MENU_GAME_RESIGN: {
                        if (!is_gaming) return 0;
                        is_gaming = false;
                        current_kifu.resign();
                        EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_GRAYED);
                        EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_GRAYED);
                        do_over_button.enabled = false;
                        resign_button.enabled = false;
                        InvalidateRect(handle_window, nullptr, FALSE);
                        if (HIWORD(w_param) == 1) {
                            MessageBoxW(NULL, std::wstring(L"まで" + std::to_wstring(current_kifu.size()) + L"手で" + ((l_param == 1) ? L"先手" : L"後手") + L"の勝ち").c_str(), L"", MB_OK);
                        }
                        else {
                            MessageBoxW(NULL, std::wstring(L"まで" + std::to_wstring(current_kifu.size()) + L"手で" + (((current_kifu.size() - 1) % 2 == 0) ? L"先手" : L"後手") + L"の勝ち").c_str(), L"", MB_OK);
                        }
                        break;
                    }
                    case ID_MENU_HELP_VERSION: {
                        version_window.show_window(SW_SHOW, window_area_rectf.left + 30.0f, window_area_rectf.top + 30.0f);
                        break;
                    }
                }
                return 0;
            }
            case WM_DROPFILES: {
                HDROP handle_drop = reinterpret_cast<HDROP>(w_param);

                wchar_t filepath[MAX_PATH];
                if (DragQueryFileW(handle_drop, 0, filepath, MAX_PATH)) {
                    if (!kifu_saved) {
                        int ret = check_nosave(handle_window);
                        switch (ret) {
                            case 0: {
                                if (overwrite_save_handler()) {
                                    return 0;
                                }
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
                    if (std::wstring(filepath).empty()) {
                        break;
                    }
                    if (is_editing) {
                        SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_BOARD, NULL), MAKELPARAM(NULL, NULL));
                    }
                    hm::kifu_ver1 temp;
                    try {
                        temp.kifu_load(utf16_to_utf8(filepath));
                    }
                    catch (const std::exception &e) {
                        MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(), L"エラー", MB_OK | MB_ICONERROR);
                        break;
                    }
                    current_kifu = temp;
                    current_kifu_path = filepath;
                    current_kifu.resign();
                    kifu_current_turn = current_kifu.size() - 1;
                    kifu_saved = true;
                    config_ref.open_file = utf16_to_utf8(filepath);
                    hm::kifuver1_to_board(current_kifu, board);
                    float visible_top = kifu_scroll_offset.y;
                    float visible_bottom = kifu_scroll_offset.y + (kifu_area_rect.bottom - kifu_area_rect.top);
                    float turn_pos = (kifu_current_turn + 1) * config_ref.kifu_spacing;

                    if (turn_pos < visible_top) {
                        kifu_scroll_offset.y = turn_pos;
                    } else if (turn_pos > visible_bottom - config_ref.kifu_spacing) {
                        kifu_scroll_offset.y = turn_pos - (kifu_area_rect.bottom - kifu_area_rect.top) + config_ref.kifu_spacing;
                    }

                    if (kifu_scroll_offset.y < 0.0f) kifu_scroll_offset.y = 0.0f;
                    update_title();
                    initialize_scroll();
                    InvalidateRect(handle_window, nullptr, FALSE);
                    DragFinish(handle_drop);
                    return 0;
                }
                return 0;
            }
            case WM_MOUSEWHEEL: {
                POINT point = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };
                ScreenToClient(handle_window, &point);

                if (PtInRect(&grid_area_rect, point)) {
                    short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                    if (key_held[VK_CONTROL]) {
                        float new_spacing = config_ref.grid_spacing + delta * 0.05f * (config_ref.grid_spacing / 50.0f);
                        if (new_spacing < 10.0f) {
                            new_spacing = 10.0f;
                        }
                        else if (new_spacing > 200.0f) {
                            new_spacing = 200.0f;
                        }
                        float mouse_x = static_cast<float>(point.x);
                        float mouse_y = static_cast<float>(point.y);
                        float board_x = (mouse_x - grid_area_rectf.left + grid_scroll_offset.x) / config_ref.grid_spacing;
                        float board_y = (grid_area_rectf.bottom - grid_scroll_offset.y - mouse_y) / config_ref.grid_spacing;
                        float scale = new_spacing / config_ref.grid_spacing;
                        grid_scroll_offset.x = (board_x * new_spacing) - (mouse_x - grid_area_rectf.left);
                        grid_scroll_offset.y = (grid_area_rectf.bottom - mouse_y) - (board_y * new_spacing);
                        config_ref.grid_spacing = new_spacing;
                    }
                    else if (key_held[VK_SHIFT]) {
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
            case WM_RBUTTONDOWN: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

                if (PtInRect(&grid_area_rect, pt)) {
                    current_focus = focus::grid;
                    if (is_editing) {
                        float gx = static_cast<float>(pt.x);
                        float gy = static_cast<float>(pt.y);
                        float fx = (gx - grid_area_rectf.left + main_window.grid_scroll_offset.x) / config_ref.grid_spacing - 0.5f;
                        float fy = (grid_area_rectf.bottom - main_window.grid_scroll_offset.y - gy) / config_ref.grid_spacing - 0.5f;
                        int x = static_cast<int>(std::round(fx));
                        int y = static_cast<int>(std::round(fy));
                        try {
                            main_window.board.set(x, y, 0);
                            InvalidateRect(handle_window, nullptr, FALSE);
                        } catch (...) {
                            return 0;
                        }
                        return 0;
                    }
                    return 0;
                }
                else if (PtInRect(&kifu_area_rect, pt)) {
                    current_focus = focus::kifu;
                    return 0;
                }

                return 0;
            }
            case WM_LBUTTONDOWN: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

                if (PtInRect(&grid_area_rect, pt)) {
                    current_focus = focus::grid;
                    if (is_gaming) {
                        float gx = static_cast<float>(pt.x);
                        float gy = static_cast<float>(pt.y);
                        float fx = (gx - grid_area_rectf.left + main_window.grid_scroll_offset.x) / config_ref.grid_spacing - 0.5f;
                        float fy = (grid_area_rectf.bottom - main_window.grid_scroll_offset.y - gy) / config_ref.grid_spacing - 0.5f;
                        int x = static_cast<int>(std::round(fx));
                        int y = static_cast<int>(std::round(fy));
                        if (current_kifu.size() == 0 && !(x == 0 && y == 0)) {
                            return 0;
                        }
                        try {
                            if (!hm::kifuver1_to_board(current_kifu).has_piece(x, y)) {
                                main_window.board.progress(x, y);
                                kifu_current_turn = current_kifu.size();
                                current_kifu.add(x, y);
                                hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);
                                InvalidateRect(handle_window, nullptr, FALSE);
                                unsigned int res = board.is_win();
                                if (res) {
                                    SendMessageW(handle_window, WM_COMMAND, MAKEWPARAM(ID_MENU_GAME_RESIGN, 1), res);
                                }
                            }
                        } catch (...) {
                            return 0;
                        }
                        return 0;
                    }
                    if (is_editing) {
                        float gx = static_cast<float>(pt.x);
                        float gy = static_cast<float>(pt.y);
                        float fx = (gx - grid_area_rectf.left + main_window.grid_scroll_offset.x) / config_ref.grid_spacing - 0.5f;
                        float fy = (grid_area_rectf.bottom - main_window.grid_scroll_offset.y - gy) / config_ref.grid_spacing - 0.5f;
                        int x = static_cast<int>(std::round(fx));
                        int y = static_cast<int>(std::round(fy));
                        try {
                            if (key_held[VK_CONTROL]) {
                                main_window.board.set(x, y, 2);
                                InvalidateRect(handle_window, nullptr, FALSE);
                            }
                            else {
                                main_window.board.set(x, y, 1);
                                InvalidateRect(handle_window, nullptr, FALSE);
                            }
                        } catch (...) {
                            return 0;
                        }
                        return 0;
                    }
                    return 0;
                }
                else if (PtInRect(&kifu_area_rect, pt)) {
                    current_focus = focus::kifu;
                    float relative_y = pt.y - kifu_area_rect.top + kifu_scroll_offset.y;
                    int clicked_turn = static_cast<int>(relative_y / config_ref.kifu_spacing) - 1;
                    if (clicked_turn < static_cast<int>(current_kifu.size())) {
                        kifu_current_turn = clicked_turn;
                        hm::kifuver1_to_board(current_kifu, board, clicked_turn + 1);
                        InvalidateRect(handle_window, nullptr, FALSE);
                        return 0;
                    }
                    return 0;
                }
                else if (PtInRect(&config_area_rect, pt)) {
                    current_focus = focus::config;
                    return 0;
                }
                else {
                    current_focus = focus::none;
                }

                if (std::abs(pt.x - boundary_grid) <= tol && pt.y > config_ref.margin && pt.y < config_ref.vertical_size - config_ref.margin) {
                    is_resizing = true;
                    cr_resize_region = resize_region::grid_kifu;
                    resize_start = pt;
                    initial_grid_size = config_ref.grid_size_x;
                    initial_kifu_size = config_ref.kifu_size_x;
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                    SetCapture(handle_window);
                    return 0;
                }
                else if (std::abs(pt.x - boundary_kifu) <= tol && pt.y > config_ref.margin && pt.y < config_ref.vertical_size - config_ref.margin) {
                    is_resizing = true;
                    cr_resize_region = resize_region::kifu_config;
                    resize_start = pt;
                    initial_kifu_size = config_ref.kifu_size_x;
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                    SetCapture(handle_window);
                    return 0;
                }
                else if (std::abs(pt.x - boundary_kifu_turn) <= tol && pt.y > config_ref.margin && pt.y < config_ref.vertical_size - config_ref.margin) {
                    is_resizing = true;
                    cr_resize_region = resize_region::kifu_turn_move;
                    resize_start = pt;
                    initial_kifu_turn_size = config_ref.kifu_turn_size_x;
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                    SetCapture(handle_window);
                    return 0;
                }
                else if (std::abs(pt.y - config_ref.vertical_size) <= tol) {
                    is_resizing = true;
                    cr_resize_region = resize_region::vertical;
                    resize_start = pt;
                    initial_vertical_size = config_ref.vertical_size;
                    SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                    SetCapture(handle_window);
                    return 0;
                }
                else {
                    is_resizing = false;
                    cr_resize_region = resize_region::none;
                }

                if (PtInRect(&do_over_button.rect, pt)) {
                    do_over_button.current_state = 2;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }
                if (PtInRect(&resign_button.rect, pt)) {
                    resign_button.current_state = 2;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
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

                if (do_over_button.current_state == 2) {
                    if (!is_gaming) return 0;
                    if (kifu_current_turn > -1) {
                        kifu_current_turn = current_kifu.size() - 2;
                        current_kifu.remove();
                        if (current_kifu.size() == 0) {
                            board.clear();
                        } else {
                            hm::kifuver1_to_board(current_kifu, board, kifu_current_turn + 1);
                        }
                    }
                    do_over_button.current_state = 1;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }
                if (resign_button.current_state == 2) {
                    if (!is_gaming) return 0;
                    is_gaming = false;
                    current_kifu.resign();
                    EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_GRAYED);
                    EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_GRAYED);
                    do_over_button.enabled = false;
                    resign_button.enabled = false;
                    resign_button.current_state = 1;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    MessageBoxW(NULL, std::wstring(L"まで" + std::to_wstring(current_kifu.size()) + L"手で" + (((current_kifu.size() - 1) % 2 == 0) ? L"先手" : L"後手") + L"の勝ち").c_str(), L"", MB_OK);
                    return 0;
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
                            float new_grid_size = initial_grid_size + dx;
                            float new_kifu_size = initial_kifu_size - dx;
                            if (new_grid_size < 100.0f) {
                                new_grid_size = 100.0f;
                                new_kifu_size = initial_kifu_size - (new_grid_size - initial_grid_size);
                            }
                            if (new_kifu_size < 100.0f) {
                                new_kifu_size = 100.0f;
                                new_grid_size = initial_grid_size + (initial_kifu_size - new_kifu_size);
                            }
                            if (new_kifu_size - config_ref.kifu_turn_size_x < 50.0f) {
                                new_grid_size = initial_grid_size + (initial_kifu_size - (config_ref.kifu_turn_size_x + 50.0f));
                                new_kifu_size = config_ref.kifu_turn_size_x + 50.0f;
                            }
                            config_ref.grid_size_x = new_grid_size;
                            config_ref.kifu_size_x = new_kifu_size;
                            break;
                        }
                        case resize_region::kifu_config: {
                            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                            int dx = pt.x - resize_start.x;
                            float new_kifu_size = initial_kifu_size + dx;
                            float new_config_size = config_ref.window_size_x - config_ref.grid_size_x - new_kifu_size;
                            if (new_kifu_size < 100.0f) {
                                new_kifu_size = 100.0f;
                                new_config_size = config_ref.window_size_x - config_ref.grid_size_x - new_kifu_size;
                            }
                            if (new_config_size < 100.0f) {
                                new_config_size = 100.0f;
                                new_kifu_size = config_ref.window_size_x - config_ref.grid_size_x - new_config_size;
                            }
                            if (new_kifu_size - config_ref.kifu_turn_size_x < 50.0f) {
                                new_kifu_size = config_ref.kifu_turn_size_x + 50.0f;
                                new_config_size = config_ref.window_size_x - config_ref.grid_size_x - new_kifu_size;
                            }
                            config_ref.kifu_size_x = new_kifu_size;
                            break;
                        }
                        case resize_region::kifu_turn_move: {
                            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                            int dx = pt.x - resize_start.x;
                            float new_kifu_turn_size = initial_kifu_turn_size + dx;
                            if (new_kifu_turn_size < 50.0f) {
                                new_kifu_turn_size = 50.0f;
                            }
                            if (config_ref.kifu_size_x - new_kifu_turn_size < 50.0f) {
                                new_kifu_turn_size = config_ref.kifu_size_x - 50.0f;
                            }
                            config_ref.kifu_turn_size_x = new_kifu_turn_size;
                            break;
                        }
                        case resize_region::vertical: {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                            int dy = pt.y - resize_start.y;
                            float new_vertical_size = initial_vertical_size + dy;
                            if (new_vertical_size < 100.0f) {
                                new_vertical_size = 100.0f;
                            }
                            if (new_vertical_size > config_ref.window_size_y - 100.0f) {
                                new_vertical_size = config_ref.window_size_y - 100.0f;
                            }
                            config_ref.vertical_size = new_vertical_size;
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

                if (PtInRect(&do_over_button.rect, pt)) {
                    do_over_button.current_state = 1;
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                else {
                    do_over_button.current_state = 0;
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                if (PtInRect(&resign_button.rect, pt)) {
                    resign_button.current_state = 1;
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                else {
                    resign_button.current_state = 0;
                    InvalidateRect(handle_window, nullptr, FALSE);
                }

                if ((std::abs(pt.x - boundary_grid) <= tol || std::abs(pt.x - boundary_kifu) <= tol || std::abs(pt.x - boundary_kifu_turn) <= tol) && config_ref.margin < static_cast<float>(pt.y) && config_ref.vertical_size - config_ref.margin > static_cast<float>(pt.y)) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                }
                else if (std::abs(pt.y - config_ref.vertical_size) <= tol) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                }
                else {
                    SetCursor(LoadCursor(nullptr, IDC_ARROW));
                }

                return 0;
            }
            case WM_PAINT: {
                PAINTSTRUCT paint_struct;
                HDC handle_device_context = BeginPaint(handle_window, &paint_struct);
                d2d1_render_target->BeginDraw();
                update_title();
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
                update_rect();
                return 0;
            }
            case WM_SIZE: {
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
            case WM_GETMINMAXINFO: {
                MINMAXINFO *minmax_info = reinterpret_cast<MINMAXINFO *>(l_param);
                float min_client_height = config_ref.vertical_size + 40.0f;
                RECT window_rect = {0, 0, 100, static_cast<LONG>(min_client_height)};
                AdjustWindowRectEx(&window_rect, GetWindowLong(handle_window, GWL_STYLE), TRUE, GetWindowLong(handle_window, GWL_EXSTYLE));
                LONG min_total_height = window_rect.bottom - window_rect.top;
                minmax_info->ptMinTrackSize.y = min_total_height;
                float min_client_width = config_ref.grid_size_x + config_ref.kifu_size_x + 100.0f;
                window_rect = {0, 0, static_cast<LONG>(min_client_width), 100};
                AdjustWindowRectEx(&window_rect, GetWindowLong(handle_window, GWL_STYLE), TRUE, GetWindowLong(handle_window, GWL_EXSTYLE));
                LONG min_total_width = window_rect.right - window_rect.left;
                minmax_info->ptMinTrackSize.x = min_total_width;
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
            case WM_MOUSEMOVE: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

                if (PtInRect(&newgame_button.rect, pt)) {
                    newgame_button.current_state = 1;
                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                else {
                    newgame_button.current_state = 0;
                    InvalidateRect(handle_window, nullptr, FALSE);
                }

                return 0;
            }
            case WM_LBUTTONDOWN: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

                if (PtInRect(&newgame_button.rect, pt)) {
                    newgame_button.current_state = 2;
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }

                for (int i = 0; i < newgame_config_keys.size(); i++) {
                    if (PtInRect(&newgame_config_area_rect[i], pt)) {
                        newgame_config_state[i] = 1 - newgame_config_state[i];
                        InvalidateRect(handle_window, nullptr, FALSE);
                        return 0;
                    }
                }
                return 0;
            }
            case WM_LBUTTONUP: {
                POINT pt = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

                if (PtInRect(&newgame_button.rect, pt)) {
                    if (!main_window.kifu_saved) {
                        int ret = check_nosave(handle_window);
                        switch (ret) {
                            case 0: {
                                if (main_window.overwrite_save_handler()) {
                                    return 0;
                                }
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
                    if (main_window.is_editing) {
                        SendMessageW(main_window, WM_COMMAND, MAKEWPARAM(ID_MENU_EDIT_BOARD, NULL), MAKELPARAM(NULL, NULL));
                    }
                    main_window.current_kifu.clear();
                    wchar_t buffer[256];
                    GetWindowTextW(players_name_edit.first, buffer, 256);
                    std::string first_name = utf16_to_utf8(buffer);
                    GetWindowTextW(players_name_edit.second, buffer, 256);
                    std::string second_name = utf16_to_utf8(buffer);
                    if (newgame_config_state[0] == 1) {
                        main_window.current_kifu.set_config_struct({ first_name, second_name, main_window.board });
                    }
                    else {
                        main_window.current_kifu.set_config_struct({ first_name, second_name, hm::board_state() });
                    }
                    config_ref.first_name = first_name;
                    config_ref.second_name = second_name;
                    hm::kifuver1_to_board(main_window.current_kifu, main_window.board);
                    main_window.kifu_current_turn = -1;
                    main_window.is_gaming = true;
                    EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_ENABLED);
                    EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_ENABLED);
                    main_window.do_over_button.enabled = true;
                    main_window.resign_button.enabled = true;
                    DrawMenuBar(main_window);
                    main_window.kifu_saved = false;
                    main_window.current_kifu_path.clear();
                    config_ref.open_file.clear();
                    main_window.initialize_scroll();
                    main_window.update_title();
                    InvalidateRect(main_window, nullptr, FALSE);
                    handle_exit();
                    newgame_button.current_state = 1;
                    unsigned int res = main_window.board.is_win();
                    if (res) {
                        SendMessageW(main_window, WM_COMMAND, MAKEWPARAM(ID_MENU_GAME_RESIGN, 1), res);
                    }
                    InvalidateRect(handle_window, nullptr, FALSE);
                    return 0;
                }

                return 0;
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
                main_window.d2d1_update_text_format();
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

    LRESULT CALLBACK window_sep_board::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
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
                main_window.update_rect();
                return 0;
            }
            case WM_DPICHANGED: {
                UINT dpi_x = HIWORD(w_param);
                UINT dpi_y = LOWORD(w_param);
                update_rect();

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
                update_rect();
                if (d2d1_render_target)
                {
                    D2D1_SIZE_U size = D2D1::SizeU(width, height);
                    d2d1_render_target->Resize(size);
                }
                InvalidateRect(handle_window, nullptr, FALSE);
                return 0;
            }
            case WM_MOUSEWHEEL: {
                POINT point = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };
                ScreenToClient(handle_window, &point);

                if (PtInRect(&grid_area_rect, point)) {
                    short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                    if (key_held[VK_CONTROL]) {
                        float new_spacing = grid_spacing + delta * 0.05f * (grid_spacing / 50.0f);
                        if (new_spacing < 10.0f) {
                            new_spacing = 10.0f;
                        }
                        else if (new_spacing > 200.0f) {
                            new_spacing = 200.0f;
                        }
                        float mouse_x = static_cast<float>(point.x);
                        float mouse_y = static_cast<float>(point.y);
                        float board_x = (mouse_x - grid_area_rectf.left + grid_scroll_offset.x) / grid_spacing;
                        float board_y = (grid_area_rectf.bottom - grid_scroll_offset.y - mouse_y) / grid_spacing;
                        float scale = new_spacing / grid_spacing;
                        grid_scroll_offset.x = (board_x * new_spacing) - (mouse_x - grid_area_rectf.left);
                        grid_scroll_offset.y = (grid_area_rectf.bottom - mouse_y) - (board_y * new_spacing);
                        grid_spacing = new_spacing;
                    }
                    else if (key_held[VK_SHIFT]) {
                        grid_scroll((float)-delta / 8, 0);
                    }
                    else {
                        grid_scroll(0, (float)-delta / 8);
                    }
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
            case WM_KEYDOWN: {
                int virtual_key = static_cast<int>(w_param);
                key_held[virtual_key] = true;
                return 0;
            }
            case WM_KEYUP: {
                int virtual_key = static_cast<int>(w_param);
                key_held[virtual_key] = false;
                return 0;
            }
            case WM_TIMER: {
                if (w_param == sep_board_timer_id) {
                    if (GetForegroundWindow() != handle_window) return 0;

                    if (key_held[VK_UP]) grid_scroll(0, -scroll_speed);
                    if (key_held[VK_DOWN]) grid_scroll(0, scroll_speed);
                    if (key_held[VK_LEFT]) grid_scroll(-scroll_speed, 0);
                    if (key_held[VK_RIGHT]) grid_scroll(scroll_speed, 0);

                    if (key_held[VK_CONTROL] && key_held['H']) grid_scroll(-scroll_speed, 0);
                    if (key_held[VK_CONTROL] && key_held['L']) grid_scroll(scroll_speed, 0);
                    if (key_held[VK_CONTROL] && key_held['J']) grid_scroll(0, scroll_speed);
                    if (key_held[VK_CONTROL] && key_held['K']) grid_scroll(0, -scroll_speed);

                    InvalidateRect(handle_window, nullptr, FALSE);
                }
                return 0;
            }
            default: {
                return DefWindowProcW(handle_window, message, w_param, l_param);
            }
        }
        return DefWindowProcW(handle_window, message, w_param, l_param);
    }

    LRESULT CALLBACK window_newgame::handle_edit_name_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param, UINT_PTR subclass_id, DWORD_PTR ref_data) {
        switch (message) {
            case WM_CHAR: {
                if (std::wcschr(L"~#", static_cast<wchar_t>(w_param))) {
                    return 0;
                }
                break;
            }
            case WM_KEYDOWN: {
                if ((GetKeyState(VK_CONTROL) & 0x8000) && w_param == 'A') {
                    SendMessageW(handle_window, EM_SETSEL, 0, -1);
                    return 0;
                }
                break;
            }
            case WM_NCDESTROY: {
                RemoveWindowSubclass(handle_window, handle_edit_name_message, subclass_id);
                DefSubclassProc(handle_window, message, w_param, l_param);
                return 0;
            }
            default: {
                return DefSubclassProc(handle_window, message, w_param, l_param);
            }
        }
        return DefSubclassProc(handle_window, message, w_param, l_param);
    }

    LRESULT CALLBACK window_settings::handle_config_edit_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param, UINT_PTR subclass_id, DWORD_PTR ref_data) {
        switch (message) {
            case WM_CHAR: {
                wchar_t ch = static_cast<wchar_t>(w_param);
                if ((ch >= L'0' && ch <= L'9') || ch == L'\b' || ch == L'\r' || ch == L'\t') {
                    break;
                }
                if (ch == L'.') {
                    int len = GetWindowTextLengthW(handle_window);
                    wchar_t* buf = new wchar_t[len + 1];
                    GetWindowTextW(handle_window, buf, len + 1);
                    bool has_dot = wcschr(buf, L'.') != nullptr;
                    delete[] buf;
                    if (!has_dot) {
                        break;
                    }
                }
                return 0;
            }
            case WM_KEYDOWN: {
                if ((GetKeyState(VK_CONTROL) & 0x8000) && w_param == 'A') {
                    SendMessageW(handle_window, EM_SETSEL, 0, -1);
                    return 0;
                }
                break;
            }
            case WM_NCDESTROY: {
                RemoveWindowSubclass(handle_window, handle_config_edit_message, subclass_id);
                break;
            }
            default: {
                return DefSubclassProc(handle_window, message, w_param, l_param);
            }
        }
        return DefSubclassProc(handle_window, message, w_param, l_param);
    }
}

int WINAPI wWinMain(_In_ HINSTANCE handle_instance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
    hmgui::load_config();

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &grobal_d2d1_factory);
    if (FAILED(hr)) return false;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&grobal_d2d1_dwrite_factory));
    if (FAILED(hr)) return false;

    main_menu.create_menu();
    main_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory);
    newgame_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory, main_window);
    settings_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory, main_window);
    version_window.initialize(grobal_d2d1_factory, grobal_d2d1_dwrite_factory, main_window);
    sep_board_window.initialize(grobal_config, grobal_d2d1_factory, grobal_d2d1_dwrite_factory);
    main_window.show_window();
    main_window.update_rect();

    SetMenu(main_window, main_menu);
    EnableMenuItem(main_menu, main_menu_game_do_over, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(main_menu, main_menu_game_resign, MF_BYCOMMAND | MF_GRAYED);
    main_window.do_over_button.enabled = false;
    main_window.resign_button.enabled = false;
    DrawMenuBar(main_window);
    DragAcceptFiles(main_window, TRUE);
    hmgui::init_open_file();

    main_timer_id = SetTimer(main_window, main_timer_id, 16, nullptr);
    sep_board_timer_id = SetTimer(sep_board_window, sep_board_timer_id, 16, nullptr);

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
