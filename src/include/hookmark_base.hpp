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

    template <typename T>
    struct two_d {
        two_d(T x, T y) : x(x), y(y) {}
        two_d() : x(0), y(0) {}
        two_d(const two_d &p) : x(p.x), y(p.y) {}
        two_d &operator=(const two_d &p) {
            x = p.x;
            y = p.y;
            return *this;
        }
        two_d &operator=(const std::pair<T, T> &p) {
            x = p.first;
            y = p.second;
            return *this;
        }
        bool operator==(const two_d &p) const {
            return x == p.x && y == p.y;
        }
        bool operator!=(const two_d &p) const {
            return !(*this == p);
        }

        T x;
        T y;
    };

    class board_state {
        private:
            signed_vector<signed_vector<bool>> _has_piece;
            signed_vector<signed_vector<bool>> _is_first;

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
                if (_has_piece.need_resize(x)) {
                    _has_piece.resize(std::max(_has_piece.positive_size(), static_cast<unsigned int>(std::max(0, x) + 1)),
                                     std::max(_has_piece.negative_size(), static_cast<unsigned int>(std::max(0, -x) + 1)));
                    _is_first.resize(_has_piece.positive_size(), _has_piece.negative_size());
                }

                if (_has_piece[x].need_resize(y)) {
                    _has_piece[x].resize(std::max(_has_piece[x].positive_size(), static_cast<unsigned int>(std::max(0, y) + 1)),
                                        std::max(_has_piece[x].negative_size(), static_cast<unsigned int>(std::max(0, -y) + 1)));
                    _is_first[x].resize(_has_piece[x].positive_size(), _has_piece[x].negative_size());
                }
            }

        public:
            board_state() {}

            board_state(const board_state &other) : _has_piece(other._has_piece), _is_first(other._is_first) {}

            void operator=(const board_state &other) {
                _has_piece = other._has_piece;
                _is_first = other._is_first;
                _current_turn = other._current_turn;
            }

            void progress(int x, int y) {
                ensure_size(x, y);
                if (_has_piece[x][y]) {
                    throw std::runtime_error("Piece already exists.");
                }
                _current_turn++;
                _has_piece[x][y] = true;
                _is_first[x][y] = (_current_turn % 2 == 1);
            }

            const signed_vector<signed_vector<bool>> &has_piece() const {
                return _has_piece;
            }

            const signed_vector<signed_vector<bool>> &is_first() const {
                return _is_first;
            }

            bool empty() const {
                return _has_piece.empty();
            }

            bool need_resize(int x, int y) const {
                if (_has_piece.need_resize(x)) {
                    return true;
                }
                else {
                    return _has_piece[x].need_resize(y);
                }
            }

            bool has_piece(int x, int y) {
                ensure_size(x, y);
                return _has_piece[x][y];
            }

            void set(int x, int y, unsigned int p) {
                ensure_size(x, y);
                _has_piece[x][y] = (p != 0);
                _is_first[x][y] = (p == 1);
            }

            unsigned int get(int x, int y) {
                ensure_size(x, y);
                if (_has_piece.need_resize(x) || _has_piece[x].need_resize(y)) return 0;
                if (!_has_piece[x][y]) return 0;
                return _is_first[x][y] ? 1 : 2;
            }

            unsigned int current_turn() const {
                return _current_turn;
            }

            unsigned int current_player() const {
                return (_current_turn % 2) + 1;
            }

            void clear() {
                _has_piece.clear();
                _is_first.clear();
                _current_turn = 0;
            }

            two_d<range> board_range() const {
                bool found = false;
                int min_x = 0, max_x = 0;
                int min_y = 0, max_y = 0;

                auto x_range = _has_piece.index_range();
                for (int x = x_range.min; x <= x_range.max; ++x) {
                    if (_has_piece.need_resize(x)) continue;
                    auto y_range = _has_piece[x].index_range();
                    for (int y = y_range.min; y <= y_range.max; ++y) {
                        if (_has_piece[x].need_resize(y)) continue;
                        if (!_has_piece[x][y]) continue;

                        if (!found) {
                            min_x = max_x = x;
                            min_y = max_y = y;
                            found = true;
                        } else {
                            min_x = std::min(min_x, x);
                            max_x = std::max(max_x, x);
                            min_y = std::min(min_y, y);
                            max_y = std::max(max_y, y);
                        }
                    }
                }

                if (!found) {
                    return { range(0, 0), range(0, 0) };
                }

                return { range(min_x, max_x), range(min_y, max_y) };
            }

            unsigned int is_win() const {
                int min_y = INT_MAX, max_y = INT_MIN;
                auto x_range = _has_piece.index_range();
                for (int x = x_range.min; x <= x_range.max; ++x) {
                    if (_has_piece.need_resize(x)) continue;
                    auto y_range = _has_piece[x].index_range();
                    min_y = std::min(min_y, y_range.min);
                    max_y = std::max(max_y, y_range.max);
                }

                for (int rotate_index = 0; rotate_index < 4; rotate_index++) {
                    const auto &mask = hookmark[rotate_index];
                    int mask_height = static_cast<int>(mask.size());
                    int mask_width = static_cast<int>(mask[0].size());

                    for (int x = x_range.min; x <= x_range.max - mask_width + 1; ++x) {
                        for (int y = min_y; y <= max_y - mask_height + 1; ++y) {
                            for (unsigned int player = 1; player <= 2; ++player) {
                                bool match = true;
                                for (int dy = 0; dy < mask_height; ++dy) {
                                    for (int dx = 0; dx < mask_width; ++dx) {
                                        if (!mask[dy][dx]) continue;
                                        int bx = x + dx;
                                        int by = y + (mask_height - 1 - dy);
                                        if (_has_piece.need_resize(bx) || _has_piece[bx].need_resize(by)) {
                                            match = false;
                                            break;
                                        }
                                        if (!_has_piece[bx][by]) {
                                            match = false;
                                            break;
                                        }
                                        bool piece_is_first = _is_first[bx][by];
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
                auto range = _has_piece.index_range();
                for (int i = range.min; i <= range.max; i++) {
                    auto row_range = _has_piece[i].index_range();
                    for (int j = row_range.min; j <= row_range.max; j++) {
                        if (!_has_piece[i][j]) {
                            result.push_back(0);
                        } else {
                            result.push_back(_is_first[i][j] ? 1 : 2);
                        }
                    }
                }
                return result;
            }
    };
}

#endif
