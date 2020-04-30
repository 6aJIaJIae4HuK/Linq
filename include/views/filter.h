#pragma once

#include <memory>
#include <iterator>

template<typename ParentIt, typename Predicate>
class FilteredIterator {
private:
	ParentIt Begin;
	ParentIt End;
	ParentIt It;
	Predicate Pred;
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename ParentIt::value_type;
	using difference_type = typename ParentIt::difference_type;
	using pointer = const value_type*;
	using reference = const value_type&;

	FilteredIterator() = default;

	FilteredIterator(const FilteredIterator&) = default;
	FilteredIterator& operator=(const FilteredIterator&) = default;

	FilteredIterator(FilteredIterator&&) = default;
	FilteredIterator& operator=(FilteredIterator&&) = default;

	FilteredIterator(ParentIt begin, ParentIt end, ParentIt it, Predicate pred)
		: Begin(begin)
		, End(end)
		, It(it)
		, Pred(pred)
	{
		AdvanceUntilPredOrEnd();
	}

	FilteredIterator& operator++() {
		++It;
		AdvanceUntilPredOrEnd();
		return *this;
	}

	FilteredIterator operator++(int) {
		FilteredIterator res = *this;
		operator++();
		return res;
	}

	FilteredIterator& operator--() {
		--It;
		AdvanceUntilPredOrBeforeBegin();
		return *this;
	}

	FilteredIterator operator--(int) {
		FilteredIterator res = *this;
		operator--();
		return res;
	}

	reference operator*() const {
		return *It;
	}

	pointer operator->() const {
		return &*It;
	}

	bool operator==(const FilteredIterator& other) const {
		return It == other.It;
	}

	bool operator!=(const FilteredIterator& other) const {
		return !(*this == other);
	}
private:
	void AdvanceUntilPredOrEnd() {
		while (It != End && !Pred(*It)) {
			++It;
		}
	}

	void AdvanceUntilPredOrBeforeBegin() {
		while (It == End || !Pred(*It)) {
			if (It == Begin) {
				It = {};
				break;
			}
			--It;
		}
	}
};

template<typename ParentView, typename Predicate>
class FilteredView {
private:
	std::shared_ptr<ParentView> Parent;
	Predicate Pred;
public:
	FilteredView(std::shared_ptr<ParentView> parent, Predicate pred)
		: Parent(parent)
		, Pred(pred)
	{}

	auto begin() const {
		return FilteredIterator(Parent->begin(), Parent->end(), Parent->begin(), Pred);
	}

	auto end() const {
		return FilteredIterator(Parent->begin(), Parent->end(), Parent->end(), Pred);
	}

	auto rbegin() const {
		return std::make_reverse_iterator(end());
	}

	auto rend() const {
		return std::make_reverse_iterator(begin());
	}
};
