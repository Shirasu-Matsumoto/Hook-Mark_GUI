#include <hookmark.hpp>
#include <hookmark_gui_window.hpp>

hmgui::window_main main_window;
hmgui::wc_main main_window_class;
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
                        main_menu_edit_comment(ID_MENU_EDIT_COMMENT),
                        main_menu_edit_kifu_info(ID_MENU_EDIT_KIFU_INFO),
                        main_menu_view_board_separate_window(ID_MENU_VIEW_BOARD_SEPARATE_WINDOW),
                        main_menu_game_new(ID_MENU_GAME_NEW),
                        main_menu_help_version(ID_MENU_HELP_VERSION);
RECT grid_area_rect = {10, 10, 490, 490};
hm::kifu_ver1 kifu;
std::wstring current_kifu_path;
hmgui::window_conf config;

bool ctrl_down = false;
bool key_held[256] = {};
UINT_PTR timer_id = 1;
static const int scroll_speed = 3;

std::string utf16_to_utf8(const std::wstring &utf16_str) {
    if (utf16_str.empty()) return std::string();

    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), static_cast<int>(utf16_str.size()), nullptr, 0, nullptr, nullptr);
    if (utf8_len <= 0) return std::string();

    std::string utf8_str(utf8_len, 0);
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), static_cast<int>(utf16_str.size()), &utf8_str[0], utf8_len, nullptr, nullptr);
    return utf8_str;
}

std::wstring utf8_to_utf16(const std::string &utf8_str) {
    if (utf8_str.empty()) return std::wstring();

    int utf16_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), nullptr, 0);
    if (utf16_len <= 0) return std::wstring();

    std::wstring utf16_str(utf16_len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), &utf16_str[0], utf16_len);
    return utf16_str;
}

// unused
void update_title() {
    if (current_kifu_path.empty()) {
        SetWindowTextW(main_window, L"Hook-Mark GUI");
    } else {
        std::filesystem::path path(current_kifu_path);
        std::wstring filename = path.filename().wstring();
        std::wstring title = L"Hook-Mark GUI - " + filename;
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

bool to_bool(const std::string& str) {
    return (str == "1" || str == "true" || str == "TRUE");
}

void import_config_single(const std::string &filepath, std::unordered_map<std::string, std::string> &config) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '[') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t\""));
        value.erase(value.find_last_not_of(" \t\"") + 1);

        if (config.find(key) == config.end()) {
            config[key] = value;
        }
    }
}

void debug(const std::wstring &str) {
    MessageBoxW(NULL, str.c_str(), L"DEBUG", MB_OK | MB_ICONINFORMATION);
    return;
}

void debug(float value) {
    MessageBoxW(NULL, std::to_wstring(value).c_str(), L"DEBUG", MB_OK | MB_ICONINFORMATION);
    return;
}

void import_config() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::filesystem::path exe_path(utf16_to_utf8(path));
    std::filesystem::path dir = exe_path.parent_path();
    std::string init_path = (dir / "config" / "init.cfg").string();
    std::string default_path = (dir / "config" / "default.cfg").string();
    if (!std::filesystem::exists(init_path) || !std::filesystem::exists(default_path)) {
        return;
    }
    std::unordered_map<std::string, std::string> um_config;
    import_config_single(default_path, um_config);
    import_config_single(init_path, um_config);
    if (um_config.count("window_pos_x")) config.window_pos_x = to_float(um_config.at("window_pos_x"));
    if (um_config.count("window_pos_y")) config.window_pos_y = to_float(um_config.at("window_pos_y"));
    if (um_config.count("margin")) config.margin = to_float(um_config.at("margin"));
    if (um_config.count("window_size_x")) config.window_size_x = to_float(um_config.at("window_size_x"));
    if (um_config.count("window_size_y")) config.window_size_y = to_float(um_config.at("window_size_y"));
    if (um_config.count("grid_spacing")) config.grid_spacing = to_float(um_config.at("grid_spacing"));
    if (um_config.count("grid_size_x")) config.grid_size_x = to_float(um_config.at("grid_size_x"));
    if (um_config.count("kifu_size_x")) config.kifu_size_x = to_float(um_config.at("kifu_size_x"));
    if (um_config.count("grid_and_kifu_size_y")) config.grid_and_kifu_size_y = to_float(um_config.at("grid_and_kifu_size_y"));
    if (um_config.count("open_file")) config.open_file = um_config.at("open_file");
    if (um_config.count("first_name")) config.first_name = um_config.at("first_name");
    if (um_config.count("second_name")) config.second_name = um_config.at("second_name");
    if (um_config.count("first_time")) config.first_time = to_float(um_config.at("first_time"));
    if (um_config.count("second_time")) config.second_time = to_float(um_config.at("second_time"));
    if (um_config.count("first_countdown")) config.first_countdown = to_float(um_config.at("first_countdown"));
    if (um_config.count("second_countdown")) config.second_countdown = to_float(um_config.at("second_countdown"));
    if (um_config.count("lose_time_runs_out")) config.lose_time_runs_out = to_bool(um_config.at("lose_time_runs_out"));
}

void hmgui::menu_main::create_menu() {
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
        AppendMenuW(main_menu_edit, MF_SEPARATOR, 0, NULL);
        AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_comment, L"コメントを編集");
        AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_kifu_info, L"棋譜情報を編集");

        AppendMenuW(main_menu_view, MF_STRING, main_menu_view_board_separate_window, L"盤面を別ウィンドウで表示");

        AppendMenuW(main_menu_game, MF_STRING, main_menu_game_new, L"新規対局");

        AppendMenuW(main_menu_help, MF_STRING, main_menu_help_version, L"バージョン情報");
    }
}

LRESULT CALLBACK hmgui::window_main::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_CREATE: {
            SetTimer(handle_window, timer_id, 33, NULL);
            CW_USEDEFAULT;
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
            d2d1_brush->Release();
            handle_exit();
            return 0;
        }
        case WM_COMMAND: {
            switch (LOWORD(w_param)) {
                case ID_MENU_FILE_CREATE_NEW: {
                    break;
                }
                case ID_MENU_FILE_OPEN: {
                    std::wstring result;
                    main_window.show_file_load_dialog(result);
                    current_kifu_path = result;
                    std::string filepath = utf16_to_utf8(result);
                    if (filepath.empty()) {
                        break;
                    }
                    try {
                        kifu.kifu_load(filepath);
                    }
                    catch (std::exception e) {
                        MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(), L"Error", MB_OK | MB_ICONERROR);
                    }
                    // update_title();
                    break;
                }
                case ID_MENU_FILE_OVERWRITE_SAVE: {
                    if (current_kifu_path.empty()) {
                        break;
                    }
                    kifu.kifu_save(utf16_to_utf8(current_kifu_path));
                    break;
                }
                case ID_MENU_FILE_SAVE_AS: {
                    std::wstring result;
                    main_window.show_file_save_dialog(result);
                    current_kifu_path = result;
                    std::string filepath = utf16_to_utf8(result);
                    if (filepath.empty()) {
                        break;
                    }
                    try {
                        kifu.kifu_save(filepath);
                    }
                    catch (std::exception e) {
                        MessageBoxW(NULL, utf8_to_utf16(e.what()).c_str(), L"Error", MB_OK | MB_ICONERROR);
                    }
                    // update_title();
                    break;
                }
                case ID_MENU_FILE_CLOSE: {
                    current_kifu_path.clear();
                    kifu.clear();
                    // update_title();
                    break;
                }
                case ID_MENU_FILE_EXIT: {
                    main_window.handle_exit();
                    break;
                }
                case ID_MENU_EDIT_MOVE_FORWARD: {
                    break;
                }
                case ID_MENU_EDIT_STEP_BACK: {
                    break;
                }
                case ID_MENU_EDIT_COMMENT: {
                    break;
                }
                case ID_MENU_EDIT_KIFU_INFO: {
                    break;
                }
                case ID_MENU_VIEW_BOARD_SEPARATE_WINDOW: {
                    break;
                }
                case ID_MENU_GAME_NEW: {
                    break;
                }
                case ID_MENU_HELP_VERSION: {
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
                if (ctrl_down)
                    grid_scroll((float)-delta / 8, 0);
                else
                    grid_scroll(0, (float)-delta / 8);

                InvalidateRect(handle_window, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSEHWHEEL: {
            short delta = GET_WHEEL_DELTA_WPARAM(w_param);
            grid_scroll((float)-delta / 8, 0);
            InvalidateRect(handle_window, nullptr, FALSE);
            return 0;
        }
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
        case WM_PAINT: {
            PAINTSTRUCT paint_struct;
            HDC handle_device_context = BeginPaint(handle_window, &paint_struct);
            RECT rect;
            GetClientRect(handle_window, &rect);
            d2d1_render_target->BeginDraw();
            draw_grid();
            d2d1_render_target->EndDraw();
            HRESULT hr = EndPaint(handle_window, &paint_struct);
            return 0;
        }
        default: {
            return DefWindowProc(handle_window, message, w_param, l_param);
        }
    }
    return DefWindowProc(handle_window, message, w_param, l_param);
}

int WINAPI wWinMain(HINSTANCE handle_instance, HINSTANCE, LPWSTR, int) {
    import_config();
    main_menu.create_menu();
    main_window.initialize(config);
    main_window.show_window();
    SetMenu(main_window, main_menu);

    MSG message;
    while (true) {
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                return static_cast<int>(message.wParam);
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
}
