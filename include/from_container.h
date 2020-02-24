#pragma once

#include "linq.h"

template<typename Container, bool IsConstView>
class FromContainerCollection : public ICollection<std::remove_cv_t<typename Container::value_type>, IsConstView> {
private:
    using T = typename std::remove_cv_t<Container>::value_type;

    class IteratorConsumer : public IIteratorConsumer<T, IsConstView> {
    private:
        ItemType<Container, IsConstView> Cont;
        mutable std::conditional_t<IsConstView, typename std::remove_cv_t<Container>::const_iterator, typename std::remove_cv_t<Container>::iterator> It;
    public:
        explicit IteratorConsumer(const Container& container)
            : Cont(container)
            , It(container.begin())
        {}

        explicit IteratorConsumer(Container&& container)
            : Cont(std::move(container))
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

        ItemType<T, IsConstView> GetCurrent() const override {
            if constexpr (IsConstView) {
                return *It;
            } else {
                return std::move(*It);
            }
        }
    };

    mutable IteratorConsumer Consumer;
public:
    explicit FromContainerCollection(const Container& container)
        : Consumer(container)
    {}

    explicit FromContainerCollection(Container&& container)
        : Consumer(std::move(container))
    {}

    IIteratorConsumer<T, IsConstView>* GetConsumer() const override {
        return &Consumer;
    }
};

template<typename Container>
inline Collection<std::remove_cv_t<typename Container::value_type>, true> AsView(const Container& container) {
    using T = std::remove_cv_t<typename Container::value_type>;
    return Collection<T, true>(std::make_shared<FromContainerCollection<Container, true>>(container));
}

template<typename Container>
inline Collection<std::remove_cv_t<typename Container::value_type>, false> AsView(Container&& container) {
    using T = std::remove_cv_t<typename Container::value_type>;
    return Collection<T, false>(std::make_shared<FromContainerCollection<Container, false>>(std::move(container)));
}
