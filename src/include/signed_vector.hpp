#ifndef __SIGNED_VECTOR_HPP__
#define __SIGNED_VECTOR_HPP__

#include <vector>
#include <initializer_list>
#include <stdexcept>

namespace hm {
    template <typename T>
    class signed_vector final {
        private:
            std::vector<T> _positive;
            std::vector<T> _negative;

        public:
            signed_vector(unsigned int init_positive_size = 0, unsigned int init_negative_size = 0) noexcept {
                _positive.resize(init_positive_size);
                _negative.resize(init_negative_size);
            }

            signed_vector(std::initializer_list<T> init_list) noexcept {
                for (const auto &val : init_list) {
                    _positive.push_back(val);
                }
            }

            void resize(unsigned int new_positive_size, unsigned int new_negative_size) noexcept {
                _positive.resize(new_positive_size);
                _negative.resize(new_negative_size);
            }

            void resize(int new_size) noexcept {
                if (new_size >= 0) {
                    _positive.resize(new_size);
                } else {
                    _negative.resize(-new_size);
                }
            }

            void set_negative(std::initializer_list<T> init_list) noexcept {
                _negative.clear();
                for (const auto &val : init_list) {
                    _negative.push_back(val);
                }
            }

            void set_positive(std::initializer_list<T> init_list) noexcept {
                _positive.clear();
                for (const auto &val : init_list) {
                    _positive.push_back(val);
                }
            }

            void resize_positive(unsigned int new_positive_size) noexcept {
                _positive.resize(new_positive_size);
            }

            void resize_negative(unsigned int new_negative_size) noexcept {
                _negative.resize(new_negative_size);
            }

            T &positive_at(unsigned int index) noexcept {
                return _positive[index];
            }

            T &negative_at(unsigned int index) noexcept {
                return _negative[index];
            }

            T &at(int index) noexcept {
                if (index >= 0) {
                    return _positive[index];
                } else {
                    return _negative[-index];
                }
            }

            void push_back(const T &value) noexcept {
                _positive.push_back(value);
            }

            void push_front(const T &value) noexcept {
                _negative.push_back(value);
            }

            range index_range() const noexcept {
                return range(-_negative.size(), _positive.size());
            }

            unsigned int positive_size() const noexcept {
                return _positive.size();
            }

            unsigned int negative_size() const noexcept {
                return _negative.size();
            }

            unsigned int size() const noexcept {
                return _positive.size() + _negative.size() - 1;
            }

            void clear() noexcept {
                _positive.clear();
                _negative.clear();
            }

            T &operator[](int index) noexcept {
                if (index >= 0) {
                    return _positive[index];
                } else {
                    return _negative[-index];
                }
            }

            const T &operator[](int index) const noexcept {
                if (index >= 0) {
                    return _positive[index];
                } else {
                    return _negative[-index];
                }
            }

            bool need_resize(int index) const noexcept {
                if (index >= 0) {
                    return index >= _positive.size();
                } else {
                    return -index >= _negative.size();
                }
            }

            std::vector<T> to_vector() const noexcept {
                std::vector<T> result;
                result.reserve(_negative.size() + _positive.size());

                for (auto it = _negative.rbegin(); it != _negative.rend(); ++it) {
                    result.push_back(*it);
                }

                for (const auto &val : _positive) {
                    result.push_back(val);
                }

                return result;
            }

            typename std::vector<T>::iterator negative_begin() noexcept {
                return _negative.begin();
            }

            typename std::vector<T>::iterator negative_end() noexcept {
                return _negative.end();
            }

            typename std::vector<T>::iterator positive_begin() noexcept {
                return _positive.begin();
            }

            typename std::vector<T>::iterator positive_end() noexcept {
                return _positive.end();
            }

            typename std::vector<T>::iterator begin() noexcept {
                return _negative.end() - 1;
            }

            typename std::vector<T>::iterator end() noexcept {
                return _positive.end();
            }

            typename std::vector<T>::const_iterator negative_begin() const noexcept {
                return _negative.begin();
            }

            typename std::vector<T>::const_iterator negative_end() const noexcept {
                return _negative.end();
            }

            typename std::vector<T>::const_iterator positive_begin() const noexcept {
                return _positive.begin();
            }

            typename std::vector<T>::const_iterator positive_end() const noexcept {
                return _positive.end();
            }

            typename std::vector<T>::const_iterator begin() const noexcept {
                return _negative.end();
            }

            typename std::vector<T>::const_iterator end() const noexcept {
                return _positive.end();
            }

            int index_range_min() const noexcept {
                return -_negative.size() + 1;
            }

            int index_range_max() const noexcept {
                return _positive.size() - 1;
            }

            bool operator==(const signed_vector &other) const noexcept {
                return _positive == other._positive && _negative == other._negative;
            }

            bool operator!=(const signed_vector &other) const noexcept {
                return !(*this == other);
            }
    };
}

#endif
