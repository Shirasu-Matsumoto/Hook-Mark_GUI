#ifndef __HOOKMARK_BASE_HPP__
#define __HOOKMARK_BASE_HPP__

#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <format>
#include <unordered_map>
#include <signed_vector.hpp>

namespace hm {
    enum piece : unsigned int {
        empty = 0, first = 1, second = 2,
    };

    struct pos {
        pos(int x, int y) : x(x), y(y) {}
        pos() : x(0), y(0) {}
        pos(const pos &p) : x(p.x), y(p.y) {}
        pos &operator=(const pos &p) {
            x = p.x;
            y = p.y;
            return *this;
        }
        pos &operator=(const std::pair<int, int> &p) {
            x = p.first;
            y = p.second;
            return *this;
        }
        bool operator==(const pos &p) const {
            return x == p.x && y == p.y;
        }
        bool operator!=(const pos &p) const {
            return !(*this == p);
        }

        int x;
        int y;
    };

    class board_state {
        private:
            signed_vector<signed_vector<bool>> has_piece;
            signed_vector<signed_vector<bool>> is_first;

            unsigned int _current_turn = 0;

            const std::vector<std::vector<std::vector<bool>>> hookmark = {
                {
                    {false, true, false, false},
                    {true, false, true, true},
                    {true, false, false, false}
                },
                {
                    {true, true, false},
                    {false, false, true},
                    {false, true, false},
                    {false, true, false}
                },
                {
                    {false, false, false, true},
                    {true, true, false, true},
                    {false, false, true, false}
                },
                {
                    {false, true, false},
                    {false, true, false},
                    {true, false, false},
                    {false, true, true}
                }
            };

            void ensure_size(int x, int y) {
                if (has_piece.need_resize(x)) {
                    has_piece.resize(std::max(has_piece.positive_size(), static_cast<unsigned int>(std::max(0, x) + 1)),
                                     std::max(has_piece.negative_size(), static_cast<unsigned int>(std::max(0, -x) + 1)));
                    is_first.resize(has_piece.positive_size(), has_piece.negative_size());
                }

                if (has_piece[x].need_resize(y)) {
                    has_piece[x].resize(std::max(has_piece[x].positive_size(), static_cast<unsigned int>(std::max(0, y) + 1)),
                                        std::max(has_piece[x].negative_size(), static_cast<unsigned int>(std::max(0, -y) + 1)));
                    is_first[x].resize(has_piece[x].positive_size(), has_piece[x].negative_size());
                }
            }

        public:
            board_state() {}

            void progress(int x, int y) {
                ensure_size(x, y);
                if (has_piece[x][y]) {
                    throw std::runtime_error("Piece already exists.");
                }
                _current_turn++;
                has_piece[x][y] = true;
                is_first[x][y] = (_current_turn % 2 == 1);
            }

            void set(int x, int y, unsigned int p) {
                ensure_size(x, y);
                has_piece[x][y] = (p != 0);
                is_first[x][y] = (p == 1);
            }

            unsigned int get(int x, int y) const {
                if (has_piece.need_resize(x) || has_piece[x].need_resize(y)) return 0;
                if (!has_piece[x][y]) return 0;
                return is_first[x][y] ? 1 : 2;
            }

            unsigned int current_turn() const {
                return _current_turn;
            }

            unsigned int current_player() const {
                return (_current_turn % 2) + 1;
            }

            void clear() {
                has_piece.clear();
                is_first.clear();
                _current_turn = 0;
            }

            unsigned int is_win() const {
                for (int rotate_index = 0; rotate_index < 4; rotate_index++) {
                    const auto &mask = hookmark[rotate_index];
                    int mask_height = static_cast<int>(mask.size());
                    int mask_width = static_cast<int>(mask[0].size());

                    auto x_range = has_piece.index_range();
                    for (int x = x_range.min; x <= x_range.max - mask_width; ++x) {
                        for (int y = x_range.min; y <= x_range.max - mask_height; ++y) {
                            for (unsigned int player = 1; player <= 2; ++player) {
                                bool match = true;
                                for (int dy = 0; dy < mask_height; ++dy) {
                                    for (int dx = 0; dx < mask_width; ++dx) {
                                        if (!mask[dy][dx]) continue;

                                        if (has_piece.need_resize(x + dx) ||
                                            has_piece[x + dx].need_resize(y + dy)) {
                                            match = false;
                                            break;
                                        }

                                        if (!has_piece[x + dx][y + dy]) {
                                            match = false;
                                            break;
                                        }

                                        bool piece_is_first = is_first[x + dx][y + dy];
                                        if ((player == 1 && !piece_is_first) || (player == 2 && piece_is_first)) {
                                            match = false;
                                            break;
                                        }
                                    }
                                    if (!match) break;
                                }

                                if (match) {
                                    return player;
                                }
                            }
                        }
                    }
                }
                return 0;
            }

            std::vector<unsigned int> to_flat_vector() const {
                std::vector<unsigned int> result;
                auto range = has_piece.index_range();
                for (int i = range.min; i <= range.max; i++) {
                    auto row_range = has_piece[i].index_range();
                    for (int j = row_range.min; j <= row_range.max; j++) {
                        if (!has_piece[i][j]) {
                            result.push_back(0);
                        } else {
                            result.push_back(is_first[i][j] ? 1 : 2);
                        }
                    }
                }
                return result;
            }
    };
}

#endif
