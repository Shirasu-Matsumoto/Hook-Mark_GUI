#ifndef __HOOKMARK_GUI_D2D1_GRID_HPP__
#define __HOOKMARK_GUI_D2D1_GRID_HPP__

#include <hookmark_gui_window_base.hpp>

namespace hmgui {
    class grid {
        public:
            const D2D1_RECT_F grid_area = D2D1::RectF(10.0f, 10.0f, 490.0f, 490.0f);
            D2D1_POINT_2F scroll_offset = D2D1::Point2F(0.0f, 0.0f);

            void draw(ID2D1RenderTarget* target, ID2D1Brush* brush, const RECT& clientRect) {
                if (!target || !brush) return;

                target->BeginDraw();

                target->Clear(D2D1::ColorF(D2D1::ColorF::White));

                for (float x = grid_area.left; x <= grid_area.right; x += 15.0f) {
                    target->DrawLine(
                        D2D1::Point2F(x - scroll_offset.x, grid_area.top - scroll_offset.y),
                        D2D1::Point2F(x - scroll_offset.x, grid_area.bottom - scroll_offset.y),
                        brush
                    );
                }

                for (float y = grid_area.top; y <= grid_area.bottom; y += 15.0f) {
                    target->DrawLine(
                        D2D1::Point2F(grid_area.left - scroll_offset.x, y - scroll_offset.y),
                        D2D1::Point2F(grid_area.right - scroll_offset.x, y - scroll_offset.y),
                        brush
                    );
                }

                target->EndDraw();
            }

            void scroll(float dx, float dy) {
                scroll_offset.x += dx;
                scroll_offset.y += dy;
            }

            void set_scroll(float x, float y) {
                scroll_offset.x = x;
                scroll_offset.y = y;
            }

            D2D1_POINT_2F get_scroll() const {
                return scroll_offset;
            }
    };
}

#endif