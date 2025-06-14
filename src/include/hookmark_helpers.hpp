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
}

#endif
