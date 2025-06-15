#ifndef __HOOKMARK_KIFU_BASE_HPP__
#define __HOOKMARK_KIFU_BASE_HPP__

#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>

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

            virtual void remove() {
                if (!_kifu.empty()) _kifu.pop_back();
            }

            virtual void clear() {
                _kifu.clear();
            }

            virtual void kifu_save(const std::string &filename) const {}

            virtual void kifu_load(const std::string &filename) {}

            virtual pos &at(unsigned int index) {
                if (index >= _kifu.size()) {
                    throw std::out_of_range("Index out of range");
                }
                return _kifu[index];
            }

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
                return static_cast<unsigned int>(_kifu.size());
            }

            operator std::vector<pos>() const {
                return _kifu;
            }
    };
}

#endif
