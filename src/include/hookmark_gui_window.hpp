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

            window_main();
            void initialize() override;
            void create_window() override;
            LRESULT CALLBACK handle_message(HWND handle_window, UINT message, WPARAM w_param, LPARAM l_param) override;
            void draw_grid();
    };

    class menu_main final : public menu_base {
        public:
            HMENU handle_menu_file;
            HMENU handle_menu_edit;

            menu_main() : menu_base() {}
            void create_menu() override;
    };
}

#endif
