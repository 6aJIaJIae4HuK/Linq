#pragma once

#include "linq.h"

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
inline Collection<T> Collection<T>::Reverse() const {
    return Collection<T>(std::make_shared<ReversedCollection<T>>(Data));
}
