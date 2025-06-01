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

    class window_main final : public window_base {
        public:
            wc_main window_class;

            ID2D1Factory *d2d1_factory;
            ID2D1HwndRenderTarget *d2d1_render_target;
            ID2D1SolidColorBrush *d2d1_brush;
            const D2D1_RECT_F grid_area_rectf = D2D1::RectF(10.0f, 10.0f, 490.0f, 490.0f);
            D2D1_POINT_2F scroll_offset = D2D1::Point2F(0.0f, 0.0f);

            window_main() : d2d1_factory(nullptr), d2d1_render_target(nullptr), d2d1_brush(nullptr) {}
            bool d2d1_initialize();
            void initialize() override;
            void create_window() override;
            void show_file_load_dialog(std::wstring &result);
            void show_file_save_dialog(std::wstring &result);
            void draw_grid();
            void grid_scroll(float dx, float dy);
            void set_grid_scroll(float x, float y);
            void handle_exit();
            D2D1_POINT_2F get_grid_scroll() const;
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
            const D2D1_RECT_F grid_area_rectf = D2D1::RectF(10.0f, 10.0f, 490.0f, 490.0f);
            D2D1_POINT_2F scroll_offset = D2D1::Point2F(0.0f, 0.0f);

            window_new_game() : d2d1_factory(nullptr), d2d1_render_target(nullptr), d2d1_brush(nullptr) {}
            bool d2d1_initialize();
            void initialize() override;
            void create_window() override;
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
    };
}

#endif
