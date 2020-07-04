#pragma once

#include <iterator>

template<typename T>
class RepeatIterator {
private:
    const T* Item;
    int CurStep = 0;
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    RepeatIterator() = default;
    RepeatIterator(const T* item, int step)
        : Item(item)
        , CurStep(step)
    {}

    RepeatIterator(const RepeatIterator&) = default;
    RepeatIterator& operator=(const RepeatIterator&) = default;

    RepeatIterator(RepeatIterator&&) = default;
    RepeatIterator& operator=(RepeatIterator&&) = default;

    RepeatIterator& operator++() {
        ++CurStep;
        return *this;
    }

    RepeatIterator operator++(int) {
        RangeIterator res = *this;
        operator++();
        return res;
    }

    RepeatIterator& operator--() {
        --CurStep;
        return *this;
    }

    RepeatIterator operator--(int) {
        RepeatIterator res = *this;
        operator--();
        return res;
    }

    reference operator*() const {
        return *Item;
    }

    pointer operator->() const {
        return Item;
    }

    bool operator==(const RepeatIterator& other) const {
        return CurStep == other.CurStep;
    }

    bool operator!=(const RepeatIterator& other) const {
        return !(*this == other);
    }

    bool operator<(const RepeatIterator& other) const {
        return Cur < other.Cur;
    }

    bool operator>(const RepeatIterator& other) const {
        return Cur > other.Cur;
    }

    bool operator<=(const RepeatIterator& other) const {
        return Cur <= other.Cur;
    }

    bool operator>=(const RepeatIterator& other) const {
        return Cur >= other.Cur;
    }

    T operator[](difference_type n) {
        return *Item;
    }

    RepeatIterator& operator+=(difference_type n) {
        CurStep += n;
        return *this;
    }

    RepeatIterator operator+(difference_type n) {
        return RepeatIterator(Item, CurStep + n);
    }

    RepeatIterator& operator-=(difference_type n) {
        CurStep -= n;
        return *this;
    }

    RepeatIterator operator-(difference_type n) {
        return RepeatIterator(Item, CurStep - n);
    }

    difference_type operator-(RepeatIterator it) {
        return CurStep - it.CurStep;
    }
};

template<typename T>
RepeatIterator<T> operator+(typename RepeatIterator<T>::difference_type n, RepeatIterator<T> it) {
    return it + n;
}

template<typename T>
class RepeatView {
private:
    T Item;
    size_t Size;
public:
    RepeatView(T item, size_t size)
        : Item(std::move(item))
        , Size(size)
    {}

    auto begin() const {
        return RepeatIterator<T>(&Item, 0);
    }

    auto end() const {
        return RepeatIterator<T>(&Item, Size);
    }

    auto rbegin() const {
        return std::make_reverse_iterator(end());
    }

    auto rend() const {
        return std::make_reverse_iterator(begin());
    }

    size_t size() const {
        return Size;
    }
};
