#ifndef __SIGNED_VECTOR_HPP__
#define __SIGNED_VECTOR_HPP__

#include <vector>
#include <initializer_list>
#include <stdexcept>

namespace hm {
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

    template <typename T>
    class signed_vector {
        private:
            std::vector<T> _positive;
            std::vector<T> _negative;

        public:
            class iterator;
            class const_iterator;

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

            const T &positive_at(unsigned int index) const noexcept {
                return _positive[index];
            }

            const T &negative_at(unsigned int index) const noexcept {
                return _negative[index];
            }

            const T &at(int index) const noexcept {
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
                return range(-static_cast<int>(_negative.size()), static_cast<int>(_positive.size()));
            }

            unsigned int positive_size() const noexcept {
                return static_cast<unsigned int>(_positive.size());
            }

            unsigned int negative_size() const noexcept {
                return static_cast<unsigned int>(_negative.size());
            }

            unsigned int size() const noexcept {
                return static_cast<unsigned int>(_positive.size()) + static_cast<unsigned int>(_negative.size()) - 1;
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

            bool empty() const noexcept {
                return _positive.empty() && _negative.empty();
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

            iterator begin() noexcept {
                return iterator(this, index_range_min());
            }

            iterator end() noexcept {
                return iterator(this, index_range_max() + 1);
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

            const_iterator begin() const noexcept {
                return const_iterator(this, index_range_min());
            }

            const_iterator end() const noexcept {
                return const_iterator(this, index_range_max() + 1);
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

            class iterator {
                private:
                    signed_vector<T> *_vector;
                    int _index;

                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = T;
                    using difference_type = std::ptrdiff_t;
                    using pointer = T*;
                    using reference = T&;

                    iterator(signed_vector<T> *vec, int index)
                        : _vector(vec), _index(index) {}

                    reference operator*() const {
                        return _vector->at(_index);
                    }

                    pointer operator->() const {
                        return &(_vector->at(_index));
                    }

                    iterator &operator++() {
                        ++_index;
                        return *this;
                    }

                    iterator operator++(int) {
                        iterator tmp = *this;
                        ++(*this);
                        return tmp;
                    }

                    iterator &operator--() {
                        --_index;
                        return *this;
                    }

                    iterator operator--(int) {
                        iterator tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    iterator &operator+=(difference_type n) {
                        _index += static_cast<int>(n);
                        return *this;
                    }

                    iterator &operator-=(difference_type n) {
                        _index -= static_cast<int>(n);
                        return *this;
                    }

                    iterator operator+(difference_type n) const {
                        return iterator(_vector, _index + static_cast<int>(n));
                    }

                    iterator operator-(difference_type n) const {
                        return iterator(_vector, _index - static_cast<int>(n));
                    }

                    difference_type operator-(const iterator &other) const {
                        return static_cast<difference_type>(_index - other._index);
                    }

                    reference operator[](difference_type n) const {
                        return _vector->at(_index + static_cast<int>(n));
                    }

                    bool operator==(const iterator &other) const {
                        return _index == other._index && _vector == other._vector;
                    }

                    bool operator!=(const iterator &other) const {
                        return !(*this == other);
                    }

                    bool operator<(const iterator &other) const {
                        return _index < other._index;
                    }

                    bool operator>(const iterator &other) const {
                        return _index > other._index;
                    }

                    bool operator<=(const iterator &other) const {
                        return _index <= other._index;
                    }

                    bool operator>=(const iterator &other) const {
                        return _index >= other._index;
                    }
            };

            class const_iterator {
                private:
                    const signed_vector<T> *_vec;
                    int _index;

                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = T;
                    using difference_type = std::ptrdiff_t;
                    using pointer = const T*;
                    using reference = const T&;

                    const_iterator(const signed_vector<T> *vec, int index)
                        : _vec(vec), _index(index) {}

                    reference operator*() const {
                        return _vec->at(_index);
                    }

                    const_iterator &operator++() {
                        ++_index;
                        return *this;
                    }

                    const_iterator operator++(int) {
                        const_iterator tmp = *this;
                        ++(*this);
                        return tmp;
                    }

                    const_iterator &operator--() {
                        --_index;
                        return *this;
                    }

                    const_iterator operator--(int) {
                        const_iterator tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    const_iterator &operator+=(difference_type n) {
                        _index += static_cast<int>(n);
                        return *this;
                    }

                    const_iterator &operator-=(difference_type n) {
                        _index -= static_cast<int>(n);
                        return *this;
                    }

                    const_iterator operator+(difference_type n) const {
                        return const_iterator(_vec, _index + static_cast<int>(n));
                    }

                    const_iterator operator-(difference_type n) const {
                        return const_iterator(_vec, _index - static_cast<int>(n));
                    }

                    difference_type operator-(const const_iterator &other) const {
                        return static_cast<difference_type>(_index - other._index);
                    }

                    reference operator[](difference_type n) const {
                        return _vec->at(_index + static_cast<int>(n));
                    }

                    bool operator==(const const_iterator &other) const {
                        return _index == other._index && _vec == other._vec;
                    }

                    bool operator!=(const const_iterator &other) const {
                        return !(*this == other);
                    }

                    bool operator<(const const_iterator &other) const {
                        return _index < other._index;
                    }

                    bool operator>(const const_iterator &other) const {
                        return _index > other._index;
                    }

                    bool operator<=(const const_iterator &other) const {
                        return _index <= other._index;
                    }

                    bool operator>=(const const_iterator &other) const {
                        return _index >= other._index;
                    }

                    int index() const noexcept {
                        return _index;
                    }
            };
    };

    template <>
    class signed_vector<bool> {
        private:
            std::vector<bool> _positive;
            std::vector<bool> _negative;

        public:
            using reference = std::vector<bool>::reference;
            using const_reference = bool;
            class iterator;
            class const_iterator;

            signed_vector(unsigned int init_positive_size = 0, unsigned int init_negative_size = 0) noexcept {
                _positive.resize(init_positive_size);
                _negative.resize(init_negative_size);
            }

            signed_vector(std::initializer_list<bool> init_list) noexcept {
                for (auto val : init_list) {
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

            reference at(int index) {
                return index >= 0 ? _positive[index] : _negative[-index];
            }

            const_reference at(int index) const {
                return index >= 0 ? _positive[index] : _negative[-index];
            }

            reference operator[](int index) {
                return index >= 0 ? _positive[index] : _negative[-index];
            }

            const_reference operator[](int index) const {
                return index >= 0 ? _positive[index] : _negative[-index];
            }

            void push_back(bool value) {
                _positive.push_back(value);
            }

            void push_front(bool value) {
                _negative.push_back(value);
            }

            range index_range() const noexcept {
                return range(-static_cast<int>(_negative.size()) + 1, static_cast<int>(_positive.size()) - 1);
            }

            void clear() noexcept {
                _positive.clear();
                _negative.clear();
            }

            bool empty() const noexcept {
                return _positive.empty() && _negative.empty();
            }

            bool need_resize(int index) const noexcept {
                return index >= 0 ? index >= _positive.size() : -index >= _negative.size();
            }

            unsigned int positive_size() const noexcept {
                return static_cast<unsigned int>(_positive.size());
            }

            unsigned int negative_size() const noexcept {
                return static_cast<unsigned int>(_negative.size());
            }

            unsigned int size() const noexcept {
                return static_cast<unsigned int>(_positive.size()) + static_cast<unsigned int>(_negative.size()) - 1;
            }

            typename std::vector<bool>::iterator negative_begin() noexcept {
                return _negative.begin();
            }

            typename std::vector<bool>::iterator negative_end() noexcept {
                return _negative.end();
            }

            typename std::vector<bool>::iterator positive_begin() noexcept {
                return _positive.begin();
            }

            typename std::vector<bool>::iterator positive_end() noexcept {
                return _positive.end();
            }

            iterator begin() noexcept {
                return iterator(this, index_range_min());
            }

            iterator end() noexcept {
                return iterator(this, index_range_max() + 1);
            }

            typename std::vector<bool>::const_iterator negative_begin() const noexcept {
                return _negative.begin();
            }

            typename std::vector<bool>::const_iterator negative_end() const noexcept {
                return _negative.end();
            }

            typename std::vector<bool>::const_iterator positive_begin() const noexcept {
                return _positive.begin();
            }

            typename std::vector<bool>::const_iterator positive_end() const noexcept {
                return _positive.end();
            }

            const_iterator begin() const noexcept {
                return const_iterator(this, index_range_min());
            }

            const_iterator end() const noexcept {
                return const_iterator(this, index_range_max() + 1);
            }

            int index_range_min() const noexcept {
                return -static_cast<int>(_negative.size()) + 1;
            }

            int index_range_max() const noexcept {
                return static_cast<int>(_positive.size()) - 1;
            }

            bool operator==(const signed_vector &other) const noexcept {
                return _positive == other._positive && _negative == other._negative;
            }

            bool operator!=(const signed_vector &other) const noexcept {
                return !(*this == other);
            }

            std::vector<bool> to_vector() const noexcept {
                std::vector<bool> result;
                result.reserve(_negative.size() + _positive.size());

                for (auto it = _negative.rbegin(); it != _negative.rend(); ++it) {
                    result.push_back(*it);
                }

                for (bool val : _positive) {
                    result.push_back(val);
                }

                return result;
            }

            class iterator {
                private:
                    signed_vector<bool> *_vector;
                    int _index;

                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = bool;
                    using difference_type = std::ptrdiff_t;
                    using pointer = void;
                    using reference = bool;

                    iterator(signed_vector<bool> *vec, int index)
                        : _vector(vec), _index(index) {}

                    reference operator*() const {
                        return _vector->at(_index);
                    }

                    iterator &operator++() {
                        ++_index;
                        return *this;
                    }

                    iterator operator++(int) {
                        iterator tmp = *this;
                        ++(*this);
                        return tmp;
                    }

                    iterator &operator--() {
                        --_index;
                        return *this;
                    }

                    iterator operator--(int) {
                        iterator tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    iterator &operator+=(difference_type n) {
                        _index += static_cast<int>(n);
                        return *this;
                    }

                    iterator &operator-=(difference_type n) {
                        _index -= static_cast<int>(n);
                        return *this;
                    }

                    iterator operator+(difference_type n) const {
                        return iterator(_vector, _index + static_cast<int>(n));
                    }

                    iterator operator-(difference_type n) const {
                        return iterator(_vector, _index - static_cast<int>(n));
                    }

                    difference_type operator-(const iterator &other) const {
                        return static_cast<difference_type>(_index - other._index);
                    }

                    reference operator[](difference_type n) const {
                        return _vector->at(_index + static_cast<int>(n));
                    }

                    bool operator==(const iterator &other) const {
                        return _index == other._index && _vector == other._vector;
                    }

                    bool operator!=(const iterator &other) const {
                        return !(*this == other);
                    }

                    bool operator<(const iterator &other) const {
                        return _index < other._index;
                    }

                    bool operator>(const iterator &other) const {
                        return _index > other._index;
                    }

                    bool operator<=(const iterator &other) const {
                        return _index <= other._index;
                    }

                    bool operator>=(const iterator &other) const {
                        return _index >= other._index;
                    }
            };

            class const_iterator {
                private:
                    const signed_vector<bool> *_vec;
                    int _index;

                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = bool;
                    using difference_type = std::ptrdiff_t;
                    using pointer = const bool*;
                    using reference = const bool&;

                    const_iterator(const signed_vector<bool> *vec, int index)
                        : _vec(vec), _index(index) {}

                    bool operator*() const {
                        return _vec->at(_index);
                    }

                    const_iterator &operator++() {
                        ++_index;
                        return *this;
                    }

                    const_iterator operator++(int) {
                        const_iterator tmp = *this;
                        ++(*this);
                        return tmp;
                    }

                    const_iterator &operator--() {
                        --_index;
                        return *this;
                    }

                    const_iterator operator--(int) {
                        const_iterator tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    const_iterator &operator+=(difference_type n) {
                        _index += static_cast<int>(n);
                        return *this;
                    }

                    const_iterator &operator-=(difference_type n) {
                        _index -= static_cast<int>(n);
                        return *this;
                    }

                    const_iterator operator+(difference_type n) const {
                        return const_iterator(_vec, _index + static_cast<int>(n));
                    }

                    const_iterator operator-(difference_type n) const {
                        return const_iterator(_vec, _index - static_cast<int>(n));
                    }

                    difference_type operator-(const const_iterator &other) const {
                        return static_cast<difference_type>(_index - other._index);
                    }

                    bool operator[](difference_type n) const {
                        return _vec->at(_index + static_cast<int>(n));
                    }

                    bool operator==(const const_iterator &other) const {
                        return _index == other._index && _vec == other._vec;
                    }

                    bool operator!=(const const_iterator &other) const {
                        return !(*this == other);
                    }

                    bool operator<(const const_iterator &other) const {
                        return _index < other._index;
                    }

                    bool operator>(const const_iterator &other) const {
                        return _index > other._index;
                    }

                    bool operator<=(const const_iterator &other) const {
                        return _index <= other._index;
                    }

                    bool operator>=(const const_iterator &other) const {
                        return _index >= other._index;
                    }

                    int index() const noexcept {
                        return _index;
                    }
            };
    };
}

#endif
