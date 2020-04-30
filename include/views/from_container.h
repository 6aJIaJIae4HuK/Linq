#pragma once

#include "bidir.h"
#include "has_method.h"

DEFINE_HAS_METHOD(begin, Begin);
DEFINE_HAS_METHOD(end, End);
DEFINE_HAS_METHOD(rbegin, Rbegin);
DEFINE_HAS_METHOD(rend, Rend);

template<typename Container>
inline constexpr bool IsForwardIterable = HasMethodBegin<Container> && HasMethodEnd<Container>;

template<typename Container>
inline constexpr bool IsBackwardIterable = HasMethodRbegin<Container> && HasMethodRend<Container>;

template<typename Container>
class FromContainerView {
public:
	using ValueType = typename std::remove_reference_t<Container>::value_type;
	static_assert(IsForwardIterable<Container>);
private:
	static constexpr bool IsBackwardIterable = IsBackwardIterable<Container>;
	Container Cont;
public:
	explicit FromContainerView(Container&& container)
		: Cont(std::forward<Container&&>(container))
	{}

	auto begin() const {
		if constexpr (IsBackwardIterable) {
			return Cont.begin();
		}
		else {
			return BidirectionalFromInput(Cont.begin(), Cont.begin());
		}
	}

	auto end() const {
		if constexpr (IsBackwardIterable) {
			return Cont.end();
		}
		else {
			return BidirectionalFromInput(Cont.begin(), Cont.end());
		}
	}

	auto rbegin() const {
		if constexpr (IsBackwardIterable) {
			return Cont.rbegin();
		}
		else {
			return std::make_reverse_iterator(end());
		}
	}

	auto rend() const {
		if constexpr (IsBackwardIterable) {
			return Cont.rend();
		}
		else {
			return std::make_reverse_iterator(begin());
		}
	}
};
