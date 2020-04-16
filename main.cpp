#include <vector>
#include <forward_list>
#include <type_traits>
#include <iostream>
#include <memory>
#include <set>
#include <functional>
#include <optional>

class S {
private:
	std::unique_ptr<int> Value;
public:
	S(int i)
		: Value(std::make_unique<int>(i))
	{}

	S(const S&) = delete;
	S& operator=(const S&) = delete;

	S(S&& other) noexcept
		: Value(std::move(other.Value))
	{
		std::cout << "S(S&&)" << std::endl;
	}

	S& operator=(S&& other) noexcept {
		Value = std::move(other.Value);
		std::cout << "operator=(S&&)" << std::endl;
		return *this;
	}

	bool operator<(const S& other) const {
		return GetValue() < other.GetValue();
	}

	int GetValue() const {
		return *Value;
	}
};

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
		return Begin == other.Begin && It == other.It;
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

#define HAS_METHOD(name, traitName)\
	template<typename T>\
	class HasMethodTrait##traitName {\
	private:\
		template<typename U> static auto Test(int) -> decltype(std::declval<U>().name(), char{});\
		template<typename U> static int Test(...);\
	public:\
		static constexpr bool Value = sizeof(Test<T>(0)) == sizeof(char);\
	};\
	template<typename T>\
	inline static constexpr bool HasMethod##traitName = HasMethodTrait##traitName<T>::Value;

HAS_METHOD(begin, Begin);
HAS_METHOD(end, End);
HAS_METHOD(rbegin, Rbegin);
HAS_METHOD(rend, Rend);

template<typename Container>
inline constexpr bool IsForwardIterable = HasMethodBegin<Container> && HasMethodEnd<Container>;

template<typename Container>
inline constexpr bool IsBackwardIterable = HasMethodRbegin<Container> && HasMethodRend<Container>;

template<typename Container, bool IsBackwardIterable = IsBackwardIterable<Container>>
class FromContainerView;

template<typename Container>
class FromContainerView<Container, true> {
public:
	using ValueType = typename std::remove_reference_t<Container>::value_type;
	static_assert(IsForwardIterable<Container>);
private:
	Container Cont;
public:
	explicit FromContainerView(Container&& container)
		: Cont(std::forward<Container&&>(container))
	{}

	auto begin() const {
		return Cont.begin();
	}

	auto end() const {
		return Cont.end();
	}

	auto rbegin() const {
		return Cont.rbegin();
	}

	auto rend() const {
		return Cont.rend();
	}
};

template<typename Container>
class FromContainerView<Container, false> {
public:
	using ValueType = typename std::remove_reference_t<Container>::value_type;
	static_assert(IsForwardIterable<Container>);
private:
	Container Cont;
public:
	explicit FromContainerView(Container&& container)
		: Cont(std::forward<Container&&>(container))
	{}

	auto begin() const {
		return BidirectionalFromInput(Cont.begin(), Cont.begin());
	}

	auto end() const {
		return BidirectionalFromInput(Cont.begin(), Cont.end());
	}

	auto rbegin() const {
		return std::make_reverse_iterator(end());
	}

	auto rend() const {
		return std::make_reverse_iterator(begin());
	}
};

template<typename ParentView>
class ReversedView {
private:
	std::shared_ptr<ParentView> Parent;
public:
	ReversedView(std::shared_ptr<ParentView> parent)
		: Parent(parent)
	{}

	auto begin() const {
		return Parent->rbegin();
	}

	auto end() const {
		return Parent->rend();
	}

	auto rbegin() const {
		return Parent->begin();
	}

	auto rend() const {
		return Parent->end();
	}
};

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

template<typename View>
class Collection {
private:
	std::shared_ptr<View> V;
public:
	explicit Collection(std::shared_ptr<View> v)
		: V(v)
	{}

	auto Reverse() const& {
		auto view = std::make_shared<ReversedView<View>>(V);
		return Collection<ReversedView<View>>(view);
	}

	auto Reverse()&& {
		auto view = std::make_shared<ReversedView<View>>(std::move(V));
		return Collection<ReversedView<View>>(view);
	}

	template<typename Predicate>
	auto Filter(Predicate pred) const& {
		auto view = std::make_shared<FilteredView<View, Predicate>>(V, pred);
		return Collection<FilteredView<View, Predicate>>(view);
	}

	template<typename Predicate>
	auto Filter(Predicate pred)&& {
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
		return std::distance(begin(), end());
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

int main() {
	auto generate = []() {
		std::vector<S> res;
		for (int i = 2; i <= 5; ++i) {
			res.emplace_back(i);
		}
		return res;
	};
	auto v = generate();
	auto col = FromContainer(generate());
	auto reversed = col.Reverse();
	auto filtered = reversed.Filter([](const S& s) { return s.GetValue() % 3 != 0; });

	std::cout << "Original: ";
	for (const auto& s : col) {
		std::cout << s.GetValue() << ' ';
	}
	std::cout << "Size: " << col.size() << std::endl;

	std::cout << "Reversed: ";
	for (const auto& s : reversed) {
		std::cout << s.GetValue() << ' ';
	}
	std::cout << "Size: " << reversed.size() << std::endl;

	std::cout << "Filtered: ";
	for (const auto& s : filtered) {
		std::cout << s.GetValue() << ' ';
	}
	std::cout << "Size: " << filtered.size() << std::endl;
	return 0;
}
