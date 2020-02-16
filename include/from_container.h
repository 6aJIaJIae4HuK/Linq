#pragma once

#include "linq.h"

template<typename Container>
class FromContainerCollection : public ICollection<typename Container::value_type> {
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
    explicit FromContainerCollection(const Container& container)
        : Consumer(container)
    {}

    IIteratorConsumer<T>* GetConsumer() const override {
        return &Consumer;
    }
};

template<typename Container>
inline Collection<typename Container::value_type> AsCollection(const Container& container) {
    using T = typename Container::value_type;
    return Collection<T>(std::make_shared<FromContainerCollection<Container>>(container));
}
