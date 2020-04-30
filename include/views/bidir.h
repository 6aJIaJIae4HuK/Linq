#pragma once

#include <iterator>

template<typename InputIt>
class BidirectionalFromInput {
private:
	InputIt Begin;
	InputIt It;
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename InputIt::value_type;
	using difference_type = typename InputIt::difference_type;
	using pointer = const value_type*;
	using reference = const value_type&;

	BidirectionalFromInput() = default;

	BidirectionalFromInput(InputIt begin, InputIt it)
		: Begin(begin)
		, It(it)
	{}

	BidirectionalFromInput(const BidirectionalFromInput&) = default;
	BidirectionalFromInput& operator=(const BidirectionalFromInput&) = default;

	BidirectionalFromInput(BidirectionalFromInput&&) = default;
	BidirectionalFromInput& operator=(BidirectionalFromInput&&) = default;

	BidirectionalFromInput& operator++() {
		++It;
		return *this;
	}

	BidirectionalFromInput operator++(int) {
		auto res = *this;
		operator++();
		return res;
	}

	reference operator*() const {
		return *It;
	}

	pointer operator->() const {
		return &*It;
	}

	bool operator==(const BidirectionalFromInput& other) const {
		return It == other.It;
	}

	bool operator!=(const BidirectionalFromInput& other) const {
		return !(*this == other);
	}

	BidirectionalFromInput& operator--() {
		if (It == Begin) {
			It = {};
		}
		else {
			auto it = Begin;
			auto nextIt = it;
			++nextIt;
			while (nextIt != It) {
				++it;
				++nextIt;
			}
			It = it;
		}
		return *this;
	}

	BidirectionalFromInput operator--(int) {
		auto res = *this;
		operator--();
		return res;
	}
};