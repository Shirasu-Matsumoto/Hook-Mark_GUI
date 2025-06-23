#ifndef __HOOKMARK_HELPERS_HPP__
#define __HOOKMARK_HELPERS_HPP__

#include <hookmark.hpp>

namespace hm {
    inline void kifuver1_to_board(kifu_ver1 &from, board_state &to, unsigned int turn) {
        to.clear();
        for (unsigned int i = 0; i <= turn; i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
    }

    inline board_state kifuver1_to_board(kifu_ver1 &from, unsigned int turn) {
        board_state to;
        for (unsigned int i = 0; i <= turn; i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
        return to;
    }

    inline void kifuver1_to_board(kifu_ver1 &from, board_state &to) {
        to.clear();
        for (unsigned int i = 0; i < from.size(); i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
    }

    inline board_state kifuver1_to_board(kifu_ver1 &from) {
        board_state to;
        for (unsigned int i = 0; i < from.size(); i++) {
            pos temp = from[i];
            to.progress(temp.x, temp.y);
        }
        return to;
    }
}

#endif
