#pragma once

#include "linq.h"

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
template<typename Pred>
inline Collection<T> Collection<T>::Filter(Pred pred) const {
    return Collection<T>(std::make_shared<FilteredCollection<T, Pred>>(Data, pred));
}
