#pragma once

#include "linq.h"

template<typename T, bool IsConstView>
class ReversedCollection : public ICollection<T, IsConstView> {
private:
    class IteratorConsumer : public IIteratorConsumer<T, IsConstView> {
    private:
        CollectionPtr<T, IsConstView> Parent;
        IIteratorConsumer<T, IsConstView>* Consumer;
    public:
        explicit IteratorConsumer(CollectionPtr<T, IsConstView> parent)
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

        ItemType<T, IsConstView> GetCurrent() const override {
            return Consumer->GetCurrent();
        }
    };

    mutable IteratorConsumer Consumer;
public:
    ReversedCollection(CollectionPtr<T, IsConstView> parent)
        : Consumer(parent)
    {}

    IIteratorConsumer<T, IsConstView>* GetConsumer() const override {
        return &Consumer;
    }
};

template<typename T, bool IsConstView>
inline Collection<T, IsConstView> Collection<T, IsConstView>::Reverse() const {
    return Collection<T, IsConstView>(std::make_shared<ReversedCollection<T, IsConstView>>(Data));
}
