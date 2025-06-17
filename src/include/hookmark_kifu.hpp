#ifndef __HOOKMARK_KIFU_HPP__
#define __HOOKMARK_KIFU_HPP__

#include <hookmark_kifu_base.hpp>

namespace hm {
    class kifu_ver1 : public kifu_base {
        private:
            std::string _config;

        public:
            kifu_ver1() : kifu_base() {}

            void kifu_save(const std::string &filename) const override {
                std::ofstream ofs(filename);
                if (ofs.fail()) {
                    throw std::runtime_error("Failed to open file for writing");
                }

                ofs << "Hook-Mark Kifu File Version 1.0\r\n#Config\r\n" << _config << "\r\n#End\r\n#Begin\r\n";
                for (const auto &move : data()) {
                    ofs << move.x << ", " << move.y << "\r\n";
                }
                ofs << "#End\r\n";
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
                        _config += line + "\r\n";
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

                if (!_config.empty() && _config.back() == '\n') {
                    _config.pop_back();
                }

                if (!header_found) {
                    throw std::runtime_error("Header not found: Invalid kifu file format");
                }

                ifs.close();
            }

            void clear() override {
                data().clear();
                _config.clear();
            }

            std::string config() {
                return _config;
            }

            void set_config(const std::string &config_str) {
                _config = config_str;
            }

            void add_config(const std::string &config_str) {
                _config += config_str;
            }
    };
}

#endif
