#pragma once

#include <memory>
#include <iterator>
#include <type_traits>

template<typename T, bool IsConstView>
using ItemType = std::conditional_t<IsConstView, const std::remove_cv_t<T>&, std::remove_cv_t<T>>;

template<typename T, bool IsConstView>
ItemType<T, IsConstView> PassItem(ItemType<T, IsConstView> item) {
    if constexpr (IsConstView) {
        return item;
    } else {
        return std::move(item);
    }
}

template<typename T, bool IsConstView>
class IIteratorConsumer {
public:
    virtual bool IterateToFirst() = 0;
    virtual bool IterateToNext() = 0;
    virtual bool IterateToLast() = 0;
    virtual bool IterateToPrev() = 0;
    virtual ItemType<T, IsConstView> GetCurrent() const = 0;
};

template<typename T, bool IsConstView>
class ICollection {
public:
    virtual IIteratorConsumer<T, IsConstView>* GetConsumer() const = 0;
};

template<typename T, bool IsConstView>
class Iterator {
private:
    IIteratorConsumer<T, IsConstView>* Consumer;
    bool CanConsume = true;
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using difference_type = std::ptrdiff_t;

    explicit Iterator(IIteratorConsumer<T, IsConstView>* consumer)
        : Consumer(consumer)
    {
        if (!Consumer->IterateToFirst()) {
            CanConsume = false;
        }
    }

    Iterator(IIteratorConsumer<T, IsConstView>* consumer, int)
        : Consumer(consumer)
        , CanConsume(false)
    {}

    Iterator(const Iterator&) = default;
    Iterator& operator=(const Iterator&) = default;

    Iterator& operator++() {
        if (!Consumer->IterateToNext()) {
            CanConsume = false;
        }
        return *this;
    }

    Iterator operator++(int) {
        auto res = *this;
        operator++();
        return res;
    }

    bool operator==(const Iterator& other) const {
        return Consumer == other.Consumer && CanConsume == other.CanConsume;
    }

    bool operator!=(const Iterator& other) const {
        return !(*this == other);
    }

    ItemType<T, IsConstView> operator*() const {
        return Consumer->GetCurrent();
    }

    template<typename = std::enable_if_t<!IsConstView>>
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

    template<typename = std::enable_if_t<!IsConstView>>
    ProxyHolder<typename> operator->() const {
        return ProxyHolder<typename>(Consumer->GetCurrent());
    }

    template<typename = std::enable_if_t<IsConstView>>
    const T* operator->() const {
        return &(Consumer->GetCurrent());
    }
};

template<typename T, bool IsConstView>
using CollectionPtr = std::shared_ptr<ICollection<T, IsConstView>>;

template<typename T, bool IsConstView>
class Collection {
private:
    CollectionPtr<T, IsConstView> Data;

public:
    explicit Collection(CollectionPtr<T, IsConstView> data)
        : Data(data)
    {}
    Iterator<T, IsConstView> begin() const {
        return Iterator<T, IsConstView>(Data->GetConsumer());
    }

    Iterator<T, IsConstView> end() const {
        return Iterator<T, IsConstView>(Data->GetConsumer(), {});
    }

    template<typename Pred>
    Collection<T, IsConstView> Filter(Pred pred) const;

    Collection<T, IsConstView> Reverse() const;

    template<typename OutContainer>
    OutContainer To() const {
        return OutContainer(begin(), end());
    }

    template<typename OutContainer>
    operator OutContainer() const {
        return To<OutContainer>();
    }
};

template<typename Container>
Collection<std::remove_cv_t<typename Container::value_type>, true> AsView(const Container& container);

template<typename Container>
Collection<std::remove_cv_t<typename Container::value_type>, false> AsView(Container&& container);

template<typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
Collection<T, false> Range(T start, T finish, T step = 1);

template<typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
Collection<T, false> Range(T finish);

#include "filter.h"
#include "reverse.h"
#include "from_container.h"
#include "range.h"
