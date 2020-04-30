#include "include/linq.h"

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

	S(const S&) = default;
	S& operator=(const S&) = default;

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

int main() {
	auto generate = []() {
		std::forward_list<S> res;
		for (int i = 2; i <= 5; ++i) {
			res.emplace_front(i);
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
