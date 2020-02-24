#pragma once

#include "linq.h"
#include <optional>

template<typename T, bool IsConstView, typename Pred>
class FilteredCollection : public ICollection<T, IsConstView> {
private:
    class IteratorConsumer : public IIteratorConsumer<T, IsConstView> {
    private:
        CollectionPtr<T, IsConstView> Parent;
        IIteratorConsumer<T, IsConstView>* Consumer;
        Pred Predicate;

        struct ProxyHolder {
        private:
            ItemType<T, IsConstView> Value;
        public:
            explicit ProxyHolder(const T& value)
                : Value(value)
            {}

            explicit ProxyHolder(T&& value)
                : Value(std::move(value))
            {}

            ItemType<T, IsConstView> ReleaseValue() {
                if constexpr (IsConstView) {
                    return Value;
                }
                else {
                    return std::move(Value);
                }
            }
        };

        mutable std::optional<ProxyHolder> Cur;

        bool HoldIfPredicateIsTrue() {
            ItemType<T, IsConstView> value = PassItem<T, IsConstView>(Consumer->GetCurrent());
            if (Predicate(value)) {
                if constexpr (IsConstView) {
                    Cur.emplace(value);
                } else {
                    Cur.emplace(std::move(value));
                }
                return true;
            }
            return false;
        }

    public:
        IteratorConsumer(CollectionPtr<T, IsConstView> parent, Pred pred)
            : Parent(parent)
            , Consumer(Parent->GetConsumer())
            , Predicate(pred)
        {}

        bool IterateToFirst() override {
            if (!Consumer->IterateToFirst())
                return false;
            if (HoldIfPredicateIsTrue())
                return true;

            while (Consumer->IterateToNext()) {
                if (HoldIfPredicateIsTrue())
                    return true;
            }
            return false;
        }

        bool IterateToNext() override {
            while (Consumer->IterateToNext()) {
                if (HoldIfPredicateIsTrue())
                    return true;
            }
            return false;
        }

        bool IterateToLast() override {
            if (!Consumer->IterateToLast())
                return false;
            if (HoldIfPredicateIsTrue())
                return true;

            while (Consumer->IterateToPrev()) {
                if (HoldIfPredicateIsTrue())
                    return true;
            }
            return false;
        }

        bool IterateToPrev() override {
            while (Consumer->IterateToPrev()) {
                if (HoldIfPredicateIsTrue())
                    return true;
            }
            return false;
        }

        ItemType<T, IsConstView> GetCurrent() const override {
            return PassItem<T, IsConstView>(Cur->ReleaseValue());
        }
    };

    mutable IteratorConsumer Consumer;
public:
    FilteredCollection(CollectionPtr<T, IsConstView> parent, Pred pred)
        : Consumer(parent, pred)
    {}

    IIteratorConsumer<T, IsConstView>* GetConsumer() const override {
        return &Consumer;
    }
};

template<typename T, bool IsConstView>
template<typename Pred>
inline Collection<T, IsConstView> Collection<T, IsConstView>::Filter(Pred pred) const {
    return Collection<T, IsConstView>(std::make_shared<FilteredCollection<T, IsConstView, Pred>>(Data, pred));
}
