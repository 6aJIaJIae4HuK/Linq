#pragma once

#include "views/views.h"

#include <memory>

template<typename View>
class Collection {
private:
	std::shared_ptr<View> V;
public:
	explicit Collection(std::shared_ptr<View> v)
		: V(v)
	{}

	auto Reverse() const & {
		auto view = std::make_shared<ReversedView<View>>(V);
		return Collection<ReversedView<View>>(view);
	}

	auto Reverse() && {
		auto view = std::make_shared<ReversedView<View>>(std::move(V));
		return Collection<ReversedView<View>>(view);
	}

	template<typename Predicate>
	auto Filter(Predicate pred) const& {
		auto view = std::make_shared<FilteredView<View, Predicate>>(V, pred);
		return Collection<FilteredView<View, Predicate>>(view);
	}

	template<typename Predicate>
	auto Filter(Predicate pred) && {
		auto view = std::make_shared<FilteredView<View, Predicate>>(std::move(V), pred);
		return Collection<FilteredView<View, Predicate>>(view);
	}

	auto begin() const {
		return V->begin();
	}

	auto end() const {
		return V->end();
	}

	auto rbegin() const {
		return V->rbegin();
	}

	auto rend() const {
		return V->rend();
	}

	size_t size() const {
		return V->size();
	}

	template<typename Container>
	Container CopyTo() const {
		return { begin(), end() };
	}

	template<typename Container>
	operator Container() const {
		return CopyTo<Container>();
	}
};

template<typename Container>
auto FromContainer(Container& container) {
	auto view = std::make_shared<FromContainerView<const Container&>>(container);
	return Collection<const FromContainerView<const Container&>>(view);
}

template<typename Container>
auto FromContainer(Container&& container) {
	auto view = std::make_shared<FromContainerView<Container>>(std::move(container));
	return Collection<FromContainerView<Container>>(view);
}

template<typename T>
auto Range(T begin, T end, T step) {
	auto view = std::make_shared<RangeView<T>>(begin, end, step);
	return Collection<RangeView<T>>(view);
}

template<typename T>
auto Range(T begin, T end) {
	return Range(begin, end, static_cast<T>(1));
}

template<typename T>
auto Range(T end) {
	return Range(static_cast<T>(0), end);
}
