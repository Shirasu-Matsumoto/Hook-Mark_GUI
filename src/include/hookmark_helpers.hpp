#ifndef __HOOKMARK_HELPERS_HPP__
#define __HOOKMARK_HELPERS_HPP__

#include <hookmark.hpp>

namespace hm {
    inline void kifuver1_to_board(kifu_ver1 &from, board_state &to, int turn) {
        to.clear();
        to = from.config_struct().initial_board;
        int draw_turn = from.size();
        if (turn <= draw_turn) {
            draw_turn = turn;
        }
        for (int i = 0; i < draw_turn; i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
    }

    inline board_state kifuver1_to_board(kifu_ver1 &from, int turn) {
        board_state to;
        to = from.config_struct().initial_board;
        int draw_turn = from.size();
        if (turn <= draw_turn) {
            draw_turn = turn;
        }
        for (int i = 0; i < draw_turn; i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
        return to;
    }

    inline void kifuver1_to_board(kifu_ver1 &from, board_state &to) {
        to.clear();
        to = from.config_struct().initial_board;
        for (unsigned int i = 0; i < from.size(); i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
    }

    inline board_state kifuver1_to_board(kifu_ver1 &from) {
        board_state to;
        to = from.config_struct().initial_board;
        for (unsigned int i = 0; i < from.size(); i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
        return to;
    }
}

#endif
