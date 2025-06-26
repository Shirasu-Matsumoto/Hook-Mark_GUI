#ifndef __HOOKMARK_KIFU_HPP__
#define __HOOKMARK_KIFU_HPP__

#include <hookmark_kifu_base.hpp>
#include <hookmark_base.hpp>
#include <windows.h>

namespace hm {
    struct kifu_config {
        std::string first;
        std::string second;
        board_state initial_board;
    };

    class kifu_ver1 : public kifu_base {
        private:
            std::string _config;
            kifu_config _config_struct;
            bool _is_resigned;

        public:
            kifu_ver1() : kifu_base() {}

            void kifu_save(const std::string &filename) const override {
                std::ofstream ofs(filename);
                if (ofs.fail()) {
                    throw std::runtime_error("Failed to open file for writing");
                }

                ofs << "Hook-Mark Kifu File Version 1.0\n#Config\n";
                if (!_config.empty()) {
                    ofs << _config;
                    if (_config.back() != '\n' && _config.back() != '\r') ofs << "\n";
                }
                const auto &bs = _config_struct.initial_board;
                if (!bs.empty()) {
                    auto [x_range, y_range] = bs.board_range();
                    int min_x = x_range.min, max_x = x_range.max;
                    int min_y = y_range.min, max_y = y_range.max;
                    if (min_x <= max_x && min_y <= max_y) {
                        ofs << "~Initial\n";
                        ofs << min_x << ", " << min_y << "\n";
                        for (int y = max_y; y >= min_y; --y) {
                            bool first = true;
                            for (int x = min_x; x <= max_x; ++x) {
                                int v = 0;
                                if (!bs.need_resize(x, y) && bs.has_piece()[x][y]) {
                                    v = bs.is_first()[x][y] ? 1 : 2;
                                }
                                if (!first) ofs << ", ";
                                ofs << v;
                                first = false;
                            }
                            ofs << "\n";
                        }
                        ofs << "~End\n";
                    }
                }

                if (!_config_struct.first.empty() && !_config_struct.second.empty()) {
                    ofs << "~Players\n";
                    ofs << _config_struct.first << "\n";
                    ofs << _config_struct.second << "\n";
                    ofs << "~End\n";
                }

                ofs << "#End\n#Begin\n";

                for (const auto &move : data()) {
                    ofs << move.x << ", " << move.y << "\n";
                }
                ofs << "#End\n";
                ofs.close();
            }

            void kifu_load(const std::string &filename) override {
                std::ifstream ifs(filename);
                if (ifs.fail()) {
                    throw std::runtime_error("Failed to open file for reading");
                }

                clear();
                bool in_kifu = false;
                bool in_config = false;
                bool header_found = false;
                std::string line;
                _config.clear();
                _config_struct = config_struct();

                std::vector<std::string> config_lines;
                bool in_initial = false, in_players = false;
                std::vector<std::string> initial_lines;
                std::vector<std::string> players_lines;

                while (std::getline(ifs, line)) {
                    std::string trimmed_line = line;
                    size_t comment_pos = trimmed_line.find('*');
                    if (comment_pos != std::string::npos) {
                        trimmed_line = line.substr(0, comment_pos);
                    }
                    trimmed_line.erase(0, trimmed_line.find_first_not_of(" \t\r\n"));
                    trimmed_line.erase(trimmed_line.find_last_not_of(" \t\r\n") + 1);

                    if (!header_found) {
                        if (line == "Hook-Mark Kifu File Version 1.0") {
                            header_found = true;
                        }
                        continue;
                    }

                    if (trimmed_line == "#Config") {
                        in_kifu = false;
                        in_config = true;
                        continue;
                    }

                    if (trimmed_line == "#Begin") {
                        in_kifu = true;
                        in_config = false;
                        continue;
                    }

                    if (trimmed_line == "#End") {
                        in_kifu = false;
                        in_config = false;
                        continue;
                    }

                    if (in_config) {
                        if (trimmed_line == "~Initial") {
                            in_initial = true;
                            in_players = false;
                            initial_lines.clear();
                            continue;
                        }
                        if (trimmed_line == "~Players") {
                            in_players = true;
                            in_initial = false;
                            players_lines.clear();
                            continue;
                        }
                        if (trimmed_line == "~End") {
                            in_initial = false;
                            in_players = false;
                            continue;
                        }
                        if (in_initial) {
                            if (!trimmed_line.empty())
                                initial_lines.push_back(trimmed_line);
                            continue;
                        }
                        if (in_players) {
                            if (!trimmed_line.empty())
                                players_lines.push_back(trimmed_line);
                            continue;
                        }
                        _config += line + "\n";
                        continue;
                    }

                    if (trimmed_line.empty()) {
                        continue;
                    }

                    if (in_kifu) {
                        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) {
                            return std::isspace(c);
                        }), line.end());

                        if (line.empty()) continue;

                        size_t comma_pos = line.find(',');
                        if (comma_pos == std::string::npos) continue;

                        try {
                            int x = std::stoi(line.substr(0, comma_pos));
                            int y = std::stoi(line.substr(comma_pos + 1));
                            data().emplace_back(x, y);
                        } catch (...) {
                            continue;
                        }
                    }
                }

                if (!players_lines.empty()) {
                    if (players_lines.size() != 2) {
                        throw std::runtime_error("The only players allowed to play are first or second.");
                    }
                    _config_struct.first = players_lines[0];
                    _config_struct.second = players_lines[1];
                }

                if (!initial_lines.empty()) {
                    if (initial_lines.size() < 2) {
                        throw std::runtime_error("~Initial must have at least 2 non-empty lines");
                    }

                    int base_x = 0, base_y = 0;
                    {
                        std::istringstream iss(initial_lines[0]);
                        char comma;
                        if (!(iss >> base_x >> comma >> base_y) || comma != ',') {
                            throw std::runtime_error("Invalid ~Initial base coordinate");
                        }
                    }

                    std::vector<std::vector<int>> board_rows;
                    size_t row_length = 0;
                    for (size_t i = 1; i < initial_lines.size(); ++i) {
                        std::vector<int> row;
                        std::istringstream iss(initial_lines[i]);
                        int v;
                        char c;
                        while (iss >> v) {
                            row.push_back(v);
                            if (iss >> c && c != ',') iss.unget();
                        }
                        if (row.empty()) {
                            throw std::runtime_error("Empty row in ~Initial board");
                        }
                        if (i == 1) row_length = row.size();
                        else if (row.size() != row_length) {
                            throw std::runtime_error("Non-rectangular ~Initial board");
                        }
                        board_rows.push_back(row);
                    }

                    hm::board_state &bs = _config_struct.initial_board;
                    bs.clear();
                    size_t height = board_rows.size();
                    for (size_t dy = 0; dy < height; ++dy) {
                        for (size_t dx = 0; dx < board_rows[dy].size(); ++dx) {
                            int v = board_rows[dy][dx];
                            if (v == 0) continue;
                            int x = base_x + static_cast<int>(dx);
                            int y = base_y + static_cast<int>(height - 1 - dy);
                            bs.set(x, y, v);
                        }
                    }
                }

                if (!_config.empty() && _config.back() == '\n') {
                    _config.pop_back();
                }

                if (!header_found) {
                    throw std::runtime_error("Header not found: Invalid kifu file format");
                }

                ifs.close();
            }

            void resign() {
                _is_resigned = true;
            }

            bool is_resigned() {
                return _is_resigned;
            }

            void clear() override {
                data().clear();
                _is_resigned = false;
                _config.clear();
            }

            std::string config() {
                return _config;
            }

            kifu_config config_struct() {
                return _config_struct;
            }

            void set_config(const std::string &config_str) {
                _config = config_str;
            }

            void add_config(const std::string &config_str) {
                _config += config_str;
            }

            void set_config_struct(const kifu_config &cs) {
                _config_struct = cs;
            }
    };
}

#endif
