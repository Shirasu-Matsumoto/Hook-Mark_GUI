#ifndef __HOOKMARK_GUI_WINDOW_HPP__
#define __HOOKMARK_GUI_WINDOW_HPP__

#include <hookmark_gui_window_base.hpp>
#include <hookmark_gui_id.hpp>
#include <hookmark_gui_version.hpp>

namespace hmgui {
    struct window_conf {
        float window_pos_x;
        float window_pos_y;
        float margin;
        float padding;
        float window_size_x;
        float window_size_y;
        float grid_spacing;
        float kifu_spacing;
        float grid_size_x;
        float kifu_size_x;
        float kifu_turn_size_x;
        float vertical_size;
        std::filesystem::path open_file;
        float label_size;

        std::string first_name;
        std::string second_name;
        float first_time;
        float second_time;
        float first_countdown;
        float second_countdown;
        bool lose_time_runs_out;

        window_conf()
            : window_pos_x(2147483648.0f),
              window_pos_y(2147483648.0f),
              margin(3.0f),
              padding(5.0f),
              window_size_x(2147483648.0f),
              window_size_y(2147483648.0f),
              grid_spacing(50.0f),
              kifu_spacing(40.0f),
              grid_size_x(500.0f),
              kifu_size_x(400.0f),
              kifu_turn_size_x(100.0f),
              vertical_size(500.0f),
              open_file(""),
              label_size(12.0f),
              first_name(""),
              second_name(""),
              first_time(600.0f),
              second_time(600.0f),
              first_countdown(10.0f),
              second_countdown(10.0f),
              lose_time_runs_out(false)
        {}
    };

    class wc_main final : public wc_base {
        public:
            wc_main();
            ATOM register_class() override;
    };

    class window_main final : public window_base {
        public:
            wc_main window_class;
            window_conf &config_ref;
            std::wstring current_kifu_path;
            hm::board_state board;
            hm::kifu_ver1 current_kifu;

            ID2D1Factory *d2d1_factory = nullptr;
            ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
            ID2D1SolidColorBrush *d2d1_brush = nullptr;
            IDWriteFactory *d2d1_dwrite_factory = nullptr;
            IDWriteTextFormat *text_format_kifu = nullptr;
            IDWriteTextFormat *text_format_label = nullptr;
            IDWriteTextFormat *text_format_config = nullptr;
            IDWriteTextFormat *text_format_button_label = nullptr;
            float scroll_speed = 10.0f;
            BOOL is_dwm_enabled = FALSE;
            RECT client_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F client_area_rectf = D2D1::RectF();
            RECT window_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F window_area_rectf = D2D1::RectF();
            RECT grid_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F grid_area_rectf = D2D1::RectF();
            D2D1_RECT_F grid_area_clip_rectf = D2D1::RectF();
            RECT kifu_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F kifu_area_rectf = D2D1::RectF();
            D2D1_RECT_F kifu_area_clip_rectf = D2D1::RectF();
            RECT config_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F config_area_rectf = D2D1::RectF();
            D2D1_RECT_F config_area_clip_rectf = D2D1::RectF();
            D2D1_POINT_2F grid_scroll_offset = D2D1::Point2F();
            D2D1_POINT_2F kifu_scroll_offset = D2D1::Point2F();
            D2D1_POINT_2F config_scroll_offset = D2D1::Point2F();
            std::vector<float> label_width;
            std::vector<float> label_height;
            int kifu_current_turn = 0;
            bool kifu_saved = true;
            d2d1_button do_over_button;
            d2d1_button resign_button;

            enum class resize_region {
                none,
                grid_kifu,
                kifu_config,
                vertical,
                kifu_turn_move
            };
            resize_region cr_resize_region = resize_region::none;
            bool is_resizing = false;
            float tol = 5.0f;
            float boundary_grid = config_ref.grid_size_x;
            float boundary_kifu = config_ref.grid_size_x + config_ref.kifu_size_x;
            float boundary_kifu_turn = config_ref.grid_size_x + config_ref.kifu_turn_size_x;
            POINT resize_start = { 0, 0 };
            float initial_grid_size = 0.0f;
            float initial_kifu_size = 0.0f;
            float initial_kifu_turn_size = 0.0f;
            float initial_vertical_size = 0.0f;

            bool is_gaming = false;
            bool is_editing = false;

            window_main(window_conf& config) : config_ref(config) {}
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            bool d2d1_update_text_format();
            void initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void update_rect();
            void update_scroll_speed();
            void initialize_scroll();
            void add_label_size(int i);
            void create_window();
            void show_file_load_dialog(std::wstring &result);
            void show_file_save_dialog(std::wstring &result);
            void redraw();
            void draw_config();
            void draw_grid();
            void draw_kifu_single(const hm::pos &move, unsigned int turn);
            void draw_kifu_single_last(unsigned int turn);
            void draw_kifu();
            void draw_board();
            void draw_game_control();
            void grid_scroll(float dx, float dy);
            void set_grid_scroll(float x, float y);
            D2D1_POINT_2F get_grid_scroll() const;
            void kifu_scroll(float dx, float dy);
            void set_kifu_scroll(float x, float y);
            D2D1_POINT_2F get_kifu_scroll() const;
            void config_scroll(float dx, float dy);
            void set_config_scroll(float x, float y);
            D2D1_POINT_2F get_config_scroll() const;
            void handle_exit();
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };

    class menu_main final : public menu_base {
        public:
            HMENU handle_menu_file = nullptr;
            HMENU handle_menu_edit = nullptr;

            menu_main() : menu_base() {}
            void create_menu() override;
    };

    class wc_newgame final : public wc_base {
        public:
            wc_newgame();
            ATOM register_class() override;
    };

    class window_newgame final : public window_base {
        public:
            wc_newgame window_class;
            window_conf &config_ref;

            ID2D1Factory *d2d1_factory = nullptr;
            IDWriteFactory *d2d1_dwrite_factory = nullptr;
            ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
            ID2D1SolidColorBrush *d2d1_brush = nullptr;
            IDWriteTextFormat *text_format_default = nullptr;
            IDWriteTextFormat *text_format_button_label = nullptr;
            D2D1_RECT_F client_area_rectf = D2D1::RectF();
            RECT client_area_rect = { 0, 0, 0, 0 };

            HFONT handle_font = nullptr;
            std::pair<HWND, HWND> players_name_edit = { nullptr, nullptr };
            std::vector<std::wstring> newgame_config_keys = {
                L"現在の局面から開始"
            };
            std::vector<D2D1_RECT_F> newgame_config_area_rectf;
            std::vector<RECT> newgame_config_area_rect;
            std::vector<int> newgame_config_state;
            size_t newgame_config_size = newgame_config_keys.size();

            d2d1_button newgame_button;

            window_newgame(window_conf &config)
                : config_ref(config),
                  newgame_config_area_rectf(newgame_config_size),
                  newgame_config_state(newgame_config_size),
                  newgame_config_area_rect(newgame_config_size)
            {};
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window);
            void create_window(HWND handle_parent_window);
            void show_window(int show_command = SW_SHOW, float x = CW_USEDEFAULT, float y = CW_USEDEFAULT);
            void update_rect();
            void redraw();
            void handle_exit();
            void release();
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };

    class wc_settings final : public wc_base {
        public:
            wc_settings();
            ATOM register_class() override;
    };

    class window_settings final : public window_base {
        public:
            wc_settings window_class;
            window_conf &config_ref;

            ID2D1Factory* d2d1_factory = nullptr;
            IDWriteFactory *d2d1_dwrite_factory = nullptr;
            ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
            ID2D1SolidColorBrush *d2d1_brush = nullptr;
            IDWriteTextFormat *text_format_default = nullptr;
            D2D1_RECT_F client_area_rectf = D2D1::RectF();
            RECT client_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F settings_area_rectf = D2D1::RectF();
            RECT settings_area_rect = { 0, 0, 0, 0 };
            float settings_item_spacing = 25.0f;

            std::vector<std::wstring> settings_item_keys = {
                L"盤面グリッドの間隔",
                L"棋譜の行間隔",
                L"盤面のラベルサイズ",
                L"盤面の横幅",
                L"棋譜の横幅",
                L"棋譜の手数の横幅",
                L"縦分割のサイズ",
                L"マージン",
                L"パディング"
            };
            std::vector<std::reference_wrapper<float>> settings_item_references = {
                config_ref.grid_spacing,
                config_ref.kifu_spacing,
                config_ref.label_size,
                config_ref.grid_size_x,
                config_ref.kifu_size_x,
                config_ref.kifu_turn_size_x,
                config_ref.vertical_size,
                config_ref.margin,
                config_ref.padding
            };
            std::vector<float> settings_item_default = {
                50.0f, 30.0f, 12.0f, 500.0f, 400.0f, 100.0f, 500.0f, 3.0f, 5.0f
            };
            std::vector<HWND> edit_controls;

            window_settings(window_conf &config) : config_ref(config) {};
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window);
            void create_edit_controls();
            void update_config();
            void update_rect();
            void create_window(HWND handle_parent_window);
            void show_window(int show_command = SW_SHOW, float x = CW_USEDEFAULT, float y = CW_USEDEFAULT);
            void redraw();
            void handle_exit();
            void release();
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };

    class wc_version final : public wc_base {
        public:
            wc_version();
            ATOM register_class() override;
    };

    class window_version final : public window_base {
        public:
            wc_version window_class;

            ID2D1Factory *d2d1_factory = nullptr;
            IDWriteFactory *d2d1_dwrite_factory = nullptr;
            ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
            ID2D1SolidColorBrush *d2d1_brush = nullptr;
            IDWriteTextFormat *text_format_default = nullptr;

            window_version() {};
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window);
            void create_window(HWND handle_parent_window);
            void show_window(int show_command = SW_SHOW, float x = CW_USEDEFAULT, float y = CW_USEDEFAULT);
            void redraw();
            void handle_exit();
            void release();
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };

    class wc_sep_board final : public wc_base {
        public:
            wc_sep_board();
            ATOM register_class() override;
    };

    class window_sep_board final : public window_base {
        public:
            wc_sep_board window_class;
            window_conf &config_ref;

            ID2D1Factory *d2d1_factory = nullptr;
            IDWriteFactory *d2d1_dwrite_factory = nullptr;
            ID2D1HwndRenderTarget *d2d1_render_target = nullptr;
            ID2D1SolidColorBrush *d2d1_brush = nullptr;
            D2D1_RECT_F client_area_rectf = D2D1::RectF();
            RECT client_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F grid_area_rectf = D2D1::RectF();
            RECT grid_area_rect = { 0, 0, 0, 0 };
            D2D1_RECT_F grid_area_clip_rectf = D2D1::RectF();
            D2D1_POINT_2F grid_scroll_offset = D2D1::Point2F(0.0f, 0.0f);
            std::vector<float> label_width;
            std::vector<float> label_height;
            IDWriteTextFormat *text_format_label = nullptr;

            hm::board_state board;
            hm::kifu_ver1 current_kifu;
            int kifu_current_turn = 0;
            float scroll_speed = 10.0f;

            window_sep_board(window_conf &config) : config_ref(config) {};
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void create_window();
            void show_window(int show_command = SW_SHOW, float x = CW_USEDEFAULT, float y = CW_USEDEFAULT, hm::board_state i_board = hm::board_state(), hm::kifu_ver1 i_kifu = hm::kifu_ver1(), unsigned int i_kifu_current_turn = 0);
            void update_rect();
            void redraw();
            void add_label_size(int i);
            void update_scroll_speed();
            void draw_grid();
            void draw_board();
            void grid_scroll(float dx, float dy);
            void set_grid_scroll(float x, float y);
            D2D1_POINT_2F get_grid_scroll() const;
            void handle_exit();
            void release();
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };
}

#endif
