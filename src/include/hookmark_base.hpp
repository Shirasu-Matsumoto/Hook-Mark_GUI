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
#include <signed_vector.hpp>

namespace hm {
    enum piece : unsigned int {
        empty, first, second,
    };

    struct range {
        int min;
        int max;

        range(int _min, int _max) : min(_min), max(_max) {}

        bool operator==(const range &other) const {
            return min == other.min && max == other.max;
        }

        bool operator!=(const range &other) const {
            return !(*this == other);
        }
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
            signed_vector<signed_vector<unsigned int>> _board;
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
            
        public:
            board_state() {}

            void set(int x, int y, unsigned int p) {
                _board[x][y] = p;
            }

            unsigned int get(int x, int y) const {
                return _board[x][y];
            }

            void progress(int x, int y, unsigned int p) {
                if (_board[x][y]) {
                    throw std::runtime_error("Piece is already exists.");
                }
                if (_board.need_resize(x)) {
                    _board.resize(std::max(_board.positive_size(), static_cast<unsigned int>(std::max(0, x) + 1)),
                                std::max(_board.negative_size(), static_cast<unsigned int>(std::max(0, -x) + 1)));
                }
                if (_board[x].need_resize(y)) {
                    _board[x].resize(std::max(_board[x].positive_size(), static_cast<unsigned int>(std::max(0, y) + 1)),
                                    std::max(_board[x].negative_size(), static_cast<unsigned int>(std::max(0, -y) + 1)));
                }
                _current_turn++;
                _board[x][y] = p;
            }

            void progress(int x, int y) {
                if (_board[x][y]) {
                    throw std::runtime_error("Piece is already exists.");
                }
                if (_board.need_resize(x)) {
                    _board.resize(std::max(_board.positive_size(), static_cast<unsigned int>(std::max(0, x) + 1)),
                                std::max(_board.negative_size(), static_cast<unsigned int>(std::max(0, -x) + 1)));
                }
                if (_board[x].need_resize(y)) {
                    _board[x].resize(std::max(_board[x].positive_size(), static_cast<unsigned int>(std::max(0, y) + 1)),
                                    std::max(_board[x].negative_size(), static_cast<unsigned int>(std::max(0, -y) + 1)));
                }
                _current_turn++;
                _board[x][y] = (_current_turn % 2) + 1;
            }

            unsigned int current_turn() const {
                return _current_turn;
            }

            unsigned int current_player() const {
                return (_current_turn % 2) + 1;
            }

            void clear() {
                _board.clear();
                _current_turn = 0;
            }

            unsigned int is_win() const {
                for (int rotate_index = 0; rotate_index < 4; rotate_index++) {
                    const auto &mask = hookmark[rotate_index];
                    int mask_height = mask.size();
                    int mask_width = mask[0].size();

                    auto x_range = _board.index_range();
                    for (int x = x_range.min; x <= x_range.max - mask_width; ++x) {
                        for (int y = x_range.min; y <= x_range.max - mask_height; ++y) {
                            for (unsigned int player = 1; player <= 2; ++player) {
                                bool match = true;
                                for (int dy = 0; dy < mask_height; ++dy) {
                                    for (int dx = 0; dx < mask_width; ++dx) {
                                        if (!mask[dy][dx]) continue;

                                        if (_board.need_resize(x + dx) ||
                                            _board[x + dx].need_resize(y + dy)) {
                                            match = false;
                                            break;
                                        }

                                        if (_board[x + dx][y + dy] != player) {
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
                auto range = _board.index_range();
                for (int i = range.min; i <= range.max; i++) {
                    std::vector<unsigned int> temp = _board[i].to_vector();
                    std::copy(temp.begin(), temp.end(), std::back_inserter(result));
                }
                return result;
            }
    };
}

#endif
