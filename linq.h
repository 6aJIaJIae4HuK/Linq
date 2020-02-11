#pragma once

#include <memory>

template<typename T>
class IIteratorConsumer {
public:
    virtual bool IterateToFirst() = 0;
    virtual bool IterateToNext() = 0;
    virtual T GetCurrent() const = 0;
};

template<typename T>
class Iterator {
private:
    IIteratorConsumer<T>& Consumer;
    bool CanConsume = true;
public:
    explicit Iterator(IIteratorConsumer<T>& consumer)
        : Consumer(consumer)
    {
        if (!Consumer.IterateToFirst()) {
            CanConsume = false;
        }
    }

    Iterator(IIteratorConsumer<T>& consumer, int)
        : Consumer(consumer)
        , CanConsume(false)
    {}

    Iterator<T>& operator++() {
        if (!Consumer.IterateToNext()) {
            CanConsume = false;
        }
        return *this;
    }

    bool operator==(const Iterator<T>& other) const {
        return &Consumer == &other.Consumer && CanConsume == other.CanConsume;
    }

    bool operator!=(const Iterator<T>& other) const {
        return !(*this == other);
    }

    T operator*() const {
        return Consumer.GetCurrent();
    }
};

template<typename T>
class ICollection {
public:
    virtual IIteratorConsumer<T>& GetConsumer() = 0;
    virtual const IIteratorConsumer<T>& GetConsumer() const = 0;
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

        T GetCurrent() const override {
            return *It;
        }
    };

    IteratorConsumer Consumer;
public:
    explicit SimpleCollection(const Container& container)
        : Consumer(container)
    {}

    const IIteratorConsumer<T>& GetConsumer() const override {
        return Consumer;
    }

    IIteratorConsumer<T>& GetConsumer() override {
        return Consumer;
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
        IIteratorConsumer<T>& Consumer;
        Pred Predicate;
    public:
        IteratorConsumer(CollectionPtr<T> parent, Pred pred)
            : Parent(parent)
            , Consumer(Parent->GetConsumer())
            , Predicate(pred)
        {}

        bool IterateToFirst() override {
            if (!Consumer.IterateToFirst())
                return false;
            if (Predicate(Consumer.GetCurrent()))
                return true;

            while (Consumer.IterateToNext()) {
                if (Predicate(Consumer.GetCurrent()))
                    return true;
            }
            return false;
        }

        bool IterateToNext() override {
            while (Consumer.IterateToNext()) {
                if (Predicate(Consumer.GetCurrent()))
                    return true;
            }
            return false;
        }

        T GetCurrent() const override {
            return Consumer.GetCurrent();
        }
    };

    IteratorConsumer Consumer;
public:
    FilteredCollection(CollectionPtr<T> parent, Pred pred)
        : Consumer(parent, pred)
    {}

    const IIteratorConsumer<T>& GetConsumer() const override {
        return Consumer;
    }

    IIteratorConsumer<T>& GetConsumer() override {
        return Consumer;
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
    Collection<T> Filter(Pred pred) {
        return Collection<T>(std::make_shared<FilteredCollection<T, Pred>>(Data, pred));
    }
};

template<typename Container>
Collection<typename Container::value_type> AsCollection(const Container& container) {
    using T = typename Container::value_type;
    return Collection<T>(std::make_shared<SimpleCollection<Container>>(container));
}
