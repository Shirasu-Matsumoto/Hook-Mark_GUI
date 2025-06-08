#ifndef __HOOKMARK_GUI_WINDOW_HPP__
#define __HOOKMARK_GUI_WINDOW_HPP__

#include <hookmark_gui_window_base.hpp>
#include <hookmark_gui_id.hpp>

namespace hmgui {
    class wc_main final : public wc_base {
        public:
            wc_main();
            ATOM register_class() override;
    };

    struct window_conf {
        float window_pos_x;
        float window_pos_y;
        float margin;
        float window_size_x;
        float window_size_y;
        float grid_spacing;
        float kifu_spacing;
        float grid_size_x;
        float kifu_size_x;
        float grid_and_kifu_size_y;
        std::string open_file;

        std::string first_name;
        std::string second_name;
        float first_time;
        float second_time;
        float first_countdown;
        float second_countdown;
        bool lose_time_runs_out;
    };

    class window_main final : public window_base {
        public:
            wc_main window_class;
            hmgui::window_conf main_config;
            std::wstring current_kifu_path;
            hm::board_state board;
            hm::kifu_ver1 current_kifu;

            ID2D1Factory *d2d1_factory;
            ID2D1HwndRenderTarget *d2d1_render_target;
            ID2D1SolidColorBrush *d2d1_brush;
            IDWriteFactory *d2d1_dwrite_factory;
            IDWriteTextFormat *text_format_default;
            IDWriteTextFormat *text_format_label;
            RECT grid_area_rect;
            D2D1_RECT_F grid_area_rectf;
            RECT kifu_area_rect;
            D2D1_RECT_F kifu_area_rectf;
            D2D1_POINT_2F grid_scroll_offset = D2D1::Point2F(0.0f, 0.0f);
            D2D1_POINT_2F kifu_scroll_offset = D2D1::Point2F(0.0f, 0.0f);
            std::vector<float> label_width;
            std::vector<float> label_height;

            window_main() : d2d1_factory(nullptr), d2d1_render_target(nullptr), d2d1_brush(nullptr) {}
            bool d2d1_initialize();
            void initialize(window_conf &config);
            void add_label_size(int i);
            void create_window();
            void show_file_load_dialog(std::wstring &result);
            void show_file_save_dialog(std::wstring &result);
            void redraw();
            void draw_grid();
            void draw_kifu();
            void grid_scroll(float dx, float dy);
            void set_grid_scroll(float x, float y);
            D2D1_POINT_2F get_grid_scroll() const;
            void kifu_scroll(float dx, float dy);
            void set_kifu_scroll(float x, float y);
            D2D1_POINT_2F get_kifu_scroll() const;
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

    class wc_new_game final : public wc_base {
        public:
            wc_new_game();
            ATOM register_class() override;
    };

    class window_new_game final : public window_base {
        public:
            wc_new_game window_class;

            ID2D1Factory *d2d1_factory;
            ID2D1HwndRenderTarget *d2d1_render_target;
            ID2D1SolidColorBrush *d2d1_brush;

            window_new_game() : d2d1_factory(nullptr), d2d1_render_target(nullptr), d2d1_brush(nullptr) {}
            bool d2d1_initialize();
            void initialize() override;
            void create_window() override;
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };
}

#endif
