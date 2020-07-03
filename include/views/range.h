#pragma once

#include <iterator>

template<typename T>
class RangeIterator {
private:
    T Cur;
    T Step;
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    RangeIterator() = default;
    RangeIterator(T cur, T step)
        : Cur(cur)
        , Step(step)
    {}

    RangeIterator(const RangeIterator&) = default;
    RangeIterator& operator=(const RangeIterator&) = default;

    RangeIterator(RangeIterator&&) = default;
    RangeIterator& operator=(RangeIterator&&) = default;

    RangeIterator& operator++() {
        Cur += Step;
        return *this;
    }

    RangeIterator operator++(int) {
        RangeIterator res = *this;
        operator++();
        return res;
    }

    RangeIterator& operator--() {
        Cur -= Step;
        return *this;
    }

    RangeIterator operator--(int) {
        RangeIterator res = *this;
        operator--();
        return res;
    }

    reference operator*() const {
        return Cur;
    }

    pointer operator->() const {
        return &Cur;
    }

    bool operator==(const RangeIterator& other) const {
        return Cur == other.Cur;
    }

    bool operator!=(const RangeIterator& other) const {
        return !(*this == other);
    }

    bool operator<(const RangeIterator& other) const {
        return Cur < other.Cur;
    }

    bool operator>(const RangeIterator& other) const {
        return Cur > other.Cur;
    }

    bool operator<=(const RangeIterator& other) const {
        return Cur <= other.Cur;
    }

    bool operator>=(const RangeIterator& other) const {
        return Cur >= other.Cur;
    }

    T operator[](difference_type n) {
        return Cur + Step * n;
    }

    RangeIterator& operator+=(difference_type n) {
        Cur += Step * n;
        return *this;
    }

    RangeIterator operator+(difference_type n) {
        return RangeIterator(Cur + Step * n, Step);
    }

    RangeIterator& operator-=(difference_type n) {
        Cur -= Step * n;
        return *this;
    }

    RangeIterator operator-(difference_type n) {
        return RangeIterator(Cur - Step * n, Step);
    }

    RangeIterator operator-(RangeIterator it) {
        return (it.Cur - Cur) / Step;
    }
};

template<typename T>
RangeIterator<T> operator+(typename RangeIterator<T>::difference_type n, RangeIterator<T> it) {
    return it + n;
}

template<typename T>
class RangeView {
private:
    T Begin;
    T End;
    T Step;
public:
    RangeView(T begin, T end, T step)
        : Begin(begin)
        , End(end)
        , Step(step)
    {
        auto diff = static_cast<RangeIterator<T>::difference_type>(End) - static_cast<RangeIterator<T>::difference_type>(Begin); // TODO: More safe actions (e.g. for [int64_t::min(), int64_t::max()))
        End = Begin + diff + !!(diff % Step);
    }

    auto begin() const {
        return RangeIterator<T>(Begin, Step);
    }

    auto end() const {
        return RangeIterator<T>(End, Step);
    }

    auto rbegin() const {
        return std::make_reverse_iterator(end());
    }

    auto rend() const {
        return std::make_reverse_iterator(begin());
    }

    size_t size() const {
        return std::distance(begin(), end());
    }
};
