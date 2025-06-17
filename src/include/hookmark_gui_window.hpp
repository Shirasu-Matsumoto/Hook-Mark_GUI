#ifndef __HOOKMARK_GUI_WINDOW_HPP__
#define __HOOKMARK_GUI_WINDOW_HPP__

#include <hookmark_gui_window_base.hpp>
#include <hookmark_gui_id.hpp>

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
        float grid_and_kifu_size_y;
        std::string open_file;
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
              grid_and_kifu_size_y(500.0f),
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
            window_conf main_config;
            std::wstring current_kifu_path;
            hm::board_state board;
            hm::kifu_ver1 current_kifu;

            ID2D1Factory *d2d1_factory;
            ID2D1HwndRenderTarget *d2d1_render_target;
            ID2D1SolidColorBrush *d2d1_brush;
            IDWriteFactory *d2d1_dwrite_factory;
            IDWriteTextFormat *text_format_kifu;
            IDWriteTextFormat *text_format_label;
            IDWriteTextFormat *text_format_config;
            IDWriteTextFormat *text_format_game_control_button_label;
            float scroll_speed = 15.0f;
            RECT client_area_rect;
            D2D1_RECT_F client_area_rectf;
            RECT window_area_rect;
            D2D1_RECT_F window_area_rectf;
            RECT grid_area_rect;
            D2D1_RECT_F grid_area_rectf;
            D2D1_RECT_F grid_area_clip_rectf;
            RECT kifu_area_rect;
            D2D1_RECT_F kifu_area_rectf;
            D2D1_RECT_F kifu_area_clip_rectf;
            RECT config_area_rect;
            D2D1_RECT_F config_area_rectf;
            D2D1_RECT_F config_area_clip_rectf;
            RECT do_over_button_area_rect;
            D2D1_RECT_F do_over_button_area_rectf;
            RECT resign_button_area_rect;
            D2D1_RECT_F resign_button_area_rectf;
            D2D1_POINT_2F grid_scroll_offset = D2D1::Point2F(0.0f, 0.0f);
            D2D1_POINT_2F kifu_scroll_offset = D2D1::Point2F(0.0f, 0.0f);
            D2D1_POINT_2F config_scroll_offset = D2D1::Point2F(0.0f, 0.0f);
            std::vector<float> label_width;
            std::vector<float> label_height;
            int kifu_current_turn = 0;
            bool kifu_saved = true;

            enum class resize_region {
                none,
                grid_kifu,
                kifu_config,
                vertical
            };
            resize_region cr_resize_region = resize_region::none;
            bool is_resizing = false;
            POINT resize_start = { 0, 0 };
            float initial_grid_size = 0.0f;
            float initial_kifu_size = 0.0f;
            float initial_grid_and_kifu_size_y = 0.0f;

            D2D1_COLOR_F red_color = D2D1::ColorF(D2D1::ColorF::Red);
            D2D1_COLOR_F blue_color = D2D1::ColorF(D2D1::ColorF::Blue);
            D2D1_COLOR_F black_color = D2D1::ColorF(D2D1::ColorF::Black);
            D2D1_COLOR_F kifu_edge_color = D2D1::ColorF(0x176ff7);
            D2D1_COLOR_F kifu_bg_color = D2D1::ColorF(0x689be9);
            D2D1_COLOR_F gray_color = D2D1::ColorF(D2D1::ColorF::Gray);
            D2D1_COLOR_F yellow_color = D2D1::ColorF(D2D1::ColorF::Yellow);

            bool is_gaming = false;

            int do_over_button_state = 0;
            int resign_button_state = 0;

            window_main() {}
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void update_rect();
            void update_config();
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
            HMENU handle_menu_file;
            HMENU handle_menu_edit;

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
            window_conf newgame_config;

            ID2D1Factory *d2d1_factory;
            IDWriteFactory *d2d1_dwrite_factory;
            ID2D1HwndRenderTarget *d2d1_render_target;
            ID2D1SolidColorBrush *d2d1_brush;
            IDWriteTextFormat *text_format_default;

            HWND handle_newgame_button = nullptr;

            window_newgame() {}
            bool d2d1_initialize(ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory);
            void initialize(window_conf &config, ID2D1Factory *i_d2d1_factory, IDWriteFactory *i_d2d1_dwrite_factory, HWND handle_parent_window);
            void create_window(HWND handle_parent_window);
            void show_window(int show_command = SW_SHOW, float x = CW_USEDEFAULT, float y = CW_USEDEFAULT);
            void redraw();
            void handle_exit();
            void release();
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };
}

#endif
