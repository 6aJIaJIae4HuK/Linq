#pragma once

#include <memory>
#include <iterator>
#include <type_traits>

template<typename T>
class IIteratorConsumer {
public:
    virtual bool IterateToFirst() = 0;
    virtual bool IterateToNext() = 0;
    virtual bool IterateToLast() = 0;
    virtual bool IterateToPrev() = 0;
    virtual T GetCurrent() const = 0;
};

template<typename T>
class ICollection {
public:
    virtual IIteratorConsumer<T>* GetConsumer() const = 0;
};

template<typename T>
class Iterator {
private:
    IIteratorConsumer<T>* Consumer;
    bool CanConsume = true;
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;

    explicit Iterator(IIteratorConsumer<T>* consumer)
        : Consumer(consumer)
    {
        if (!Consumer->IterateToFirst()) {
            CanConsume = false;
        }
    }

    Iterator(IIteratorConsumer<T>* consumer, int)
        : Consumer(consumer)
        , CanConsume(false)
    {}

    Iterator(const Iterator<T>&) = default;
    Iterator& operator=(const Iterator<T>&) = default;

    Iterator<T>& operator++() {
        if (!Consumer->IterateToNext()) {
            CanConsume = false;
        }
        return *this;
    }

    Iterator<T> operator++(int) {
        auto res = *this;
        operator++();
        return res;
    }

    bool operator==(const Iterator<T>& other) const {
        return Consumer == other.Consumer && CanConsume == other.CanConsume;
    }

    bool operator!=(const Iterator<T>& other) const {
        return !(*this == other);
    }

    T operator*() const {
        return Consumer->GetCurrent();
    }

    struct ProxyHolder {
    private:
        T Value;
    public:
        ProxyHolder(T&& value)
            : Value(std::move(value))
        {}

        const T* operator->() const {
            return &Value;
        }
    };

    ProxyHolder operator->() const {
        return ProxyHolder(Consumer->GetCurrent());
    }
};

template<typename T>
using CollectionPtr = std::shared_ptr<ICollection<T>>;

template<typename T>
class Collection {
private:
    CollectionPtr<T> Data;

public:
    explicit Collection(CollectionPtr<T> data)
        : Data(data)
    {}
    Iterator<T> begin() const {
        return Iterator<T>(Data->GetConsumer());
    }

    Iterator<T> end() const {
        return Iterator<T>(Data->GetConsumer(), {});
    }

    template<typename Pred>
    Collection<T> Filter(Pred pred) const;

    Collection<T> Reverse() const;

    template<typename OutContainer>
    OutContainer To() const {
        return OutContainer(begin(), end());
    }
};

template<typename Container>
Collection<typename Container::value_type> AsCollection(const Container& container);

template<typename T, class = typename std::enable_if_t<std::is_integral_v<T>>>
Collection<T> Range(T start, T finish, T step = 1);

template<typename T, class = typename std::enable_if_t<std::is_integral_v<T>>>
Collection<T> Range(T finish);

#include "filter.h"
#include "reverse.h"
#include "from_container.h"
#include "range.h"
