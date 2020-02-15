#pragma once

#include <memory>
#include <iterator>

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
class ICollection {
public:
    virtual IIteratorConsumer<T>* GetConsumer() const = 0;
};

template<typename Container>
class SimpleCollection : public ICollection<typename Container::value_type> {
private:
    using T = typename Container::value_type;
    
    class IteratorConsumer : public IIteratorConsumer<T> {
    private:
        const Container& Cont;
        typename Container::const_iterator It;
    public:
        explicit IteratorConsumer(const Container& container)
            : Cont(container)
            , It(container.begin())
        {}

        bool IterateToFirst() override {
            It = Cont.begin();
            return It != Cont.end();
        }

        bool IterateToNext() override {
            ++It;
            return It != Cont.end();
        }

        bool IterateToLast() override {
            auto it = Cont.rbegin();
            if (it == Cont.rend())
                return false;

            It = Cont.rbegin().base();
            --It;
            return true;
        }

        bool IterateToPrev() override {
            if (It == Cont.begin())
                return false;
            --It;
            return true;
        }

        T GetCurrent() const override {
            return *It;
        }
    };

    mutable IteratorConsumer Consumer;
public:
    explicit SimpleCollection(const Container& container)
        : Consumer(container)
    {}

    IIteratorConsumer<T>* GetConsumer() const override {
        return &Consumer;
    }
};

template<typename T>
using CollectionPtr = std::shared_ptr<ICollection<T>>;

template<typename T, typename Pred>
class FilteredCollection : public ICollection<T> {
private:

    class IteratorConsumer : public IIteratorConsumer<T> {
    private:
        CollectionPtr<T> Parent;
        IIteratorConsumer<T>* Consumer;
        Pred Predicate;
    public:
        IteratorConsumer(CollectionPtr<T> parent, Pred pred)
            : Parent(parent)
            , Consumer(Parent->GetConsumer())
            , Predicate(pred)
        {}

        bool IterateToFirst() override {
            if (!Consumer->IterateToFirst())
                return false;
            if (Predicate(Consumer->GetCurrent()))
                return true;

            while (Consumer->IterateToNext()) {
                if (Predicate(Consumer->GetCurrent()))
                    return true;
            }
            return false;
        }

        bool IterateToNext() override {
            while (Consumer->IterateToNext()) {
                if (Predicate(Consumer->GetCurrent()))
                    return true;
            }
            return false;
        }

        bool IterateToLast() override {
            if (!Consumer->IterateToLast())
                return false;
            if (Predicate(Consumer->GetCurrent()))
                return true;

            while (Consumer->IterateToPrev()) {
                if (Predicate(Consumer->GetCurrent()))
                    return true;
            }
            return false;
        }

        bool IterateToPrev() override {
            while (Consumer->IterateToPrev()) {
                if (Predicate(Consumer->GetCurrent()))
                    return true;
            }
            return false;
        }

        T GetCurrent() const override {
            return Consumer->GetCurrent();
        }
    };

    mutable IteratorConsumer Consumer;
public:
    FilteredCollection(CollectionPtr<T> parent, Pred pred)
        : Consumer(parent, pred)
    {}

    IIteratorConsumer<T>* GetConsumer() const override {
        return &Consumer;
    }
};

template<typename T>
class ReversedCollection : public ICollection<T> {
private:
    class IteratorConsumer : public IIteratorConsumer<T> {
    private:
        CollectionPtr<T> Parent;
        IIteratorConsumer<T>* Consumer;
    public:
        explicit IteratorConsumer(CollectionPtr<T> parent)
            : Parent(parent)
            , Consumer(Parent->GetConsumer())
        {}

        bool IterateToFirst() override {
            return Consumer->IterateToLast();
        }

        bool IterateToNext() override {
            return Consumer->IterateToPrev();
        }

        bool IterateToLast() override {
            return Consumer->IterateToFirst();
        }

        bool IterateToPrev() override {
            return Consumer->IterateToNext();
        }

        T GetCurrent() const override {
            return Consumer->GetCurrent();
        }
    };

    mutable IteratorConsumer Consumer;
public:
    ReversedCollection(CollectionPtr<T> parent)
        : Consumer(parent)
    {}

    IIteratorConsumer<T>* GetConsumer() const override {
        return &Consumer;
    }
};

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
    Collection<T> Filter(Pred pred) const {
        return Collection<T>(std::make_shared<FilteredCollection<T, Pred>>(Data, pred));
    }

    Collection<T> Reverse() const {
        return Collection<T>(std::make_shared<ReversedCollection<T>>(Data));
    }

    template<typename OutContainer>
    OutContainer To() const {
        return OutContainer(begin(), end());
    }
};

template<typename Container>
Collection<typename Container::value_type> AsCollection(const Container& container) {
    using T = typename Container::value_type;
    return Collection<T>(std::make_shared<SimpleCollection<Container>>(container));
}
