#ifndef __HOOKMARK_KIFU_HPP__
#define __HOOKMARK_KIFU_HPP__

#include <hookmark_base.hpp>
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

                ofs << "Hook-Mark Kifu File Version 1.0\n#Config\n" << _config << "#End\n#Begin\n";
                for (const auto &move : data()) {
                    ofs << "\t" << move.x << ", " << move.y << "\n";
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
                _config.clear(); // 初期化

                while (std::getline(ifs, line)) {
                    // コメント削除
                    std::string trimmed_line = line;
                    size_t comment_pos = trimmed_line.find('*');
                    if (comment_pos != std::string::npos) {
                        trimmed_line = line.substr(0, comment_pos);
                    }
                    trimmed_line.erase(0, trimmed_line.find_first_not_of(" \t\r\n"));
                    trimmed_line.erase(trimmed_line.find_last_not_of(" \t\r\n") + 1);

                    if (!header_found) {
                        // ヘッダー行ではコメントもタブも許容しない
                        if (line == "Hook-Mark Kifu File Version 1.0") {
                            header_found = true;
                        }
                        continue;
                    }

                    // セクション判定
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
                        // コメント除去済みの line をそのまま（整形含めて）記録
                        _config += line + "\n";
                        continue;
                    }

                    // Config外では空行をスキップ
                    if (trimmed_line.empty()) {
                        continue;
                    }

                    if (in_kifu) {
                        // 空白除去（スペース含む） → 座標として読み取る準備
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

                if (!header_found) {
                    throw std::runtime_error("Header not found: Invalid kifu file format");
                }

                ifs.close();
            }

            void clear() override {
                data().clear();
                _config.clear();
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
