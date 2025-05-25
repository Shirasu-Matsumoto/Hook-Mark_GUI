#ifndef __HOOKMARK_KIFU_HPP__
#define __HOOKMARK_KIFU_HPP__

#include <hookmark_base.hpp>

namespace hm {
    class kifu_base {
        private:
            std::vector<pos> _kifu;

        public:
            kifu_base() {}
            virtual ~kifu_base() {}

            virtual void add(int x, int y) {
                _kifu.emplace_back(x, y);
            }

            virtual void clear() {
                _kifu.clear();
            }

            virtual void kifu_save(const std::string &filename) const {}

            virtual void kifu_load(const std::string &filename) {}

            virtual bool operator==(const kifu_base &other) const {
                return _kifu == other._kifu;
            }

            virtual bool operator!=(const kifu_base &other) const {
                return !(*this == other);
            }

            virtual pos &operator[](unsigned int index) {
                if (index >= _kifu.size()) {
                    throw std::out_of_range("Index out of range");
                }
                return _kifu[index];
            }

            virtual const pos &operator[](unsigned int index) const {
                if (index >= _kifu.size()) {
                    throw std::out_of_range("Index out of range");
                }
                return _kifu[index];
            }

            virtual std::vector<pos> &data() {
                return _kifu;
            }

            virtual const std::vector<pos> &data() const {
                return _kifu;
            }

            virtual unsigned int size() const {
                return _kifu.size();
            }
    };

    class kifu_ver1 : public kifu_base {
        public:
            kifu_ver1() : kifu_base() {}

            void kifu_save(const std::string &filename) const override {
                std::ofstream ofs(filename);
                if (!ofs) {
                    throw std::runtime_error("Failed to open file for writing");
                }

                ofs << "Hook-Mark Kifu File Version 1.0\n#Begin\n";
                for (const auto &move : data()) {
                    ofs << "\t" << move.x << ", " << move.y << "\n";
                }
                ofs << "#End\n";
                ofs.close();
            }

            void kifu_load(const std::string &filename) override {
                std::ifstream ifs(filename);
                if (!ifs) {
                    throw std::runtime_error("Failed to open file for reading");
                }

                clear();
                bool in_kifu = false;
                bool header_found = false;
                std::string line;

                while (std::getline(ifs, line)) {
                    size_t comment_pos = line.find('*');
                    if (comment_pos != std::string::npos) {
                        line = line.substr(0, comment_pos);
                    }

                    line.erase(0, line.find_first_not_of(" \t\r\n"));
                    line.erase(line.find_last_not_of(" \t\r\n") + 1);

                    if (line.empty()) {
                        continue;
                    }

                    if (!header_found) {
                        if (line == "Hook-Mark Kifu File Version 1.0") {
                            header_found = true;
                        }
                        continue;
                    }

                    if (line == "#Begin") {
                        in_kifu = true;
                        continue;
                    }

                    if (line == "#End") {
                        break;
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

                if (!header_found) {
                    throw std::runtime_error("Header not found: Invalid kifu file format");
                }

                ifs.close();
            }
    };
}

#endif
