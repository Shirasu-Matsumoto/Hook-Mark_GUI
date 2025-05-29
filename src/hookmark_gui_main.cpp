#include <hookmark.hpp>
#include <hookmark_gui_window.hpp>

ID2D1Factory *d2d1_factory = nullptr;
ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
ID2D1SolidColorBrush *brush = nullptr;

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
hmgui::grid main_grid;
RECT grid_area_rect = {10, 10, 490, 490};

bool ctrl_down = false;
bool key_held[256] = {};
UINT_PTR timer_id = 1;
static const int scroll_speed = 3;

bool initialize_d2d() {
    if (d2d1_factory) return true;

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1_factory);
    if (FAILED(hr)) return false;

    RECT rect;
    GetClientRect(main_window, &rect);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
    D2D1_HWND_RENDER_TARGET_PROPERTIES handle_window_props = D2D1::HwndRenderTargetProperties(main_window, size);

    hr = d2d1_factory->CreateHwndRenderTarget(props, handle_window_props, &d2d1_render_target);
    if (FAILED(hr)) return false;

    hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
    return SUCCEEDED(hr);
}

bool initialize_d2d_with_debug() {
    if (d2d1_factory) return true;

    MessageBoxW(NULL, L"Initializing D2D...\n", L"", MB_OK);

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1_factory);
    if (FAILED(hr)) {
        MessageBoxW(NULL, L"Failed: D2D1CreateFactory\n", L"", MB_OK);
        return false;
    }

    RECT rect;
    if (!GetClientRect(main_window, &rect)) {
        MessageBoxW(NULL, L"Failed: GetClientRect\n", L"", MB_OK);
        return false;
    }

    D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
    D2D1_HWND_RENDER_TARGET_PROPERTIES handle_window_props = D2D1::HwndRenderTargetProperties(main_window, size);

    hr = d2d1_factory->CreateHwndRenderTarget(props, handle_window_props, &d2d1_render_target);
    if (FAILED(hr)) {
        MessageBoxW(NULL, L"Failed: CreateHwndRenderTarget\n", L"", MB_OK);
        return false;
    }

    hr = d2d1_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
    if (FAILED(hr)) {
        MessageBoxW(NULL, L"Failed: CreateSolidColorBrush\n", L"", MB_OK);
        return false;
    }

    MessageBoxW(NULL, L"D2D initialized successfully.\n", L"", MB_OK);
    return true;
}

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
        case WM_CREATE: {
            initialize_d2d_with_debug();
            SetTimer(main_window, timer_id, 16, NULL);
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
            KillTimer(main_window, timer_id);
            brush->Release();
            PostQuitMessage(0);
            return 0;
        }
        case WM_COMMAND: {
            switch (LOWORD(w_param)) {
                case ID_MENU_FILE_CREATE_NEW: {
                    break;
                }
                case ID_MENU_FILE_OPEN: {
                    break;
                }
                case ID_MENU_FILE_OVERWRITE_SAVE: {
                    break;
                }
                case ID_MENU_FILE_SAVE_AS: {
                    break;
                }
                case ID_MENU_FILE_EXIT: {
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
            ScreenToClient(main_window, &point);

            if (PtInRect(&grid_area_rect, point)) {
                short delta = GET_WHEEL_DELTA_WPARAM(w_param);
                if (ctrl_down)
                    main_grid.scroll((float)-delta / 8, 0);
                else
                    main_grid.scroll(0, (float)-delta / 8);

                InvalidateRect(main_window, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSEHWHEEL: {
            short delta = GET_WHEEL_DELTA_WPARAM(w_param);
            main_grid.scroll((float)-delta / 8, 0);
            InvalidateRect(main_window, nullptr, FALSE);
            return 0;
        }
        case WM_TIMER: {
            if (w_param == timer_id) {
                if (!ctrl_down) return 0;
                if (key_held['H']) main_grid.scroll(-scroll_speed, 0);
                if (key_held['L']) main_grid.scroll(scroll_speed, 0);
                if (key_held['J']) main_grid.scroll(0, scroll_speed);
                if (key_held['K']) main_grid.scroll(0, -scroll_speed);
                InvalidateRect(main_window, nullptr, FALSE);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT paint_struct;
            HDC handle_device_context = BeginPaint(main_window, &paint_struct);
            RECT rect;
            GetClientRect(main_window, &rect);
            d2d1_render_target->BeginDraw();
            main_grid.draw(d2d1_render_target, brush, rect);
            d2d1_render_target->EndDraw();
            EndPaint(main_window, &paint_struct);
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
    while (PeekMessageW(&message, nullptr, NULL, NULL, WM_NULL)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
