#include <hookmark.hpp>
#include <hookmark_gui_window.hpp>

hmgui::window_main main_window;
hmgui::wc_main main_window_class;
hmgui::menu_main main_menu;
hmgui::menu_item_popup  main_menu_file,
                        main_menu_edit,
                        main_menu_view,
                        main_menu_help;
hmgui::menu_item        main_menu_file_create_new(ID_MENU_FILE_CREATE_NEW),
                        main_menu_file_open(ID_MENU_FILE_OPEN),
                        main_menu_file_overwrite_save(ID_MENU_FILE_OVERWRITE_SAVE),
                        main_menu_file_save_as(ID_MENU_FILE_SAVE_AS),
                        main_menu_file_exit(ID_MENU_FILE_EXIT),
                        main_menu_edit_move_forward(ID_MENU_EDIT_MOVE_FORWARD),
                        main_menu_edit_step_back(ID_MENU_EDIT_STEP_BACK),
                        main_menu_edit_comment(ID_MENU_EDIT_COMMENT),
                        main_menu_edit_kifu_info(ID_MENU_EDIT_KIFU_INFO),
                        main_menu_view_board_separate_window(ID_MENU_VIEW_BOARD_SEPARATE_WINDOW),
                        main_menu_game_new(ID_MENU_GAME_NEW),
                        main_menu_help_version(ID_MENU_HELP_VERSION);

void hmgui::menu_main::create_menu() {
    handle_menu = CreateMenu();
    if (handle_menu) {
        AppendMenuW(handle_menu, MF_POPUP, main_menu_file, L"ファイル(&F)");
        AppendMenuW(handle_menu, MF_POPUP, main_menu_edit, L"編集(&E)");
        AppendMenuW(handle_menu, MF_POPUP, main_menu_view, L"表示(&V)");
        AppendMenuW(handle_menu, MF_POPUP, main_menu_help, L"ヘルプ(&H)");

        AppendMenuW(main_menu_file, MF_STRING, main_menu_file_create_new, L"新規作成(&N)");
        AppendMenuW(main_menu_file, MF_STRING, main_menu_file_open, L"開く(&O)");
        AppendMenuW(main_menu_file, MF_STRING, main_menu_file_overwrite_save, L"上書き保存(&S)");
        AppendMenuW(main_menu_file, MF_STRING, main_menu_file_save_as, L"名前を付けて保存(&A)");
        AppendMenuW(main_menu_file, MF_STRING, main_menu_file_exit, L"終了");

        AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_move_forward, L"一手進む");
        AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_step_back, L"一手戻る");
        AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_comment, L"コメントを編集");
        AppendMenuW(main_menu_edit, MF_STRING, main_menu_edit_kifu_info, L"棋譜情報を編集");

        AppendMenuW(main_menu_view, MF_STRING, main_menu_view_board_separate_window, L"盤面を別ウィンドウで表示");

        AppendMenuW(main_menu_help, MF_STRING, main_menu_help_version, L"バージョン情報");
    }
}

LRESULT CALLBACK hmgui::window_main::handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_SYSKEYDOWN: {
            switch (w_param) {
                case 'F': {
                    SendMessageW(handle_window, WM_SYSCOMMAND, SC_KEYMENU, 'F');
                }
                case 'E': {
                    SendMessageW(handle_window, WM_SYSCOMMAND, SC_KEYMENU, 'E');
                }
                case 'V': {
                    SendMessageW(handle_window, WM_SYSCOMMAND, SC_KEYMENU, 'V');
                }
                case 'H': {
                    SendMessageW(handle_window, WM_SYSCOMMAND, SC_KEYMENU, 'H');
                }
                
            }
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_COMMAND: {
            // switch (LOWORD(w_param)) {
            //     default: {
            //         break;
            //     }
            // }
            return 0;
        }
        case WM_PAINT: {
            return 0;
        }
        default: {
            return DefWindowProc(handle_window, message, w_param, l_param);
        }
    }
    return DefWindowProc(handle_window, message, w_param, l_param);
}

int WINAPI wWinMain(HINSTANCE handle_instance, HINSTANCE, LPWSTR, int) {
    main_menu.create_menu();
    main_window.initialize();
    main_window.show_window();
    SetMenu(main_window, main_menu);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
