#include "include/linq.h"
#include <vector>
#include <iostream>
#include <list>
#include <string>

class NonCopyable {
private:
    std::unique_ptr<int> Value;
public:
    explicit NonCopyable(int value)
        : Value(std::make_unique<int>(value))
    {}

    int GetValue() const {
        return *Value;
    }
};

void Sample1() {
    std::vector<std::string> v{ "2", "22", "222" };
    const auto& list = AsView(v)
        .Filter([](auto i) { return true; })
        .Reverse();
    auto it = list.begin();
    it++;
    std::cout << it->length() << std::endl;
    std::vector<int> nums = Range(-4, 11, 3);
    for (auto i : nums) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

void Sample2() {
    auto generate = []() {
        std::vector<NonCopyable> res;
        for (int i = 0; i < 5; i++) {
            res.emplace_back(i);
        }
        return res;
    };

    auto& list = 
        AsView(generate())
        .Filter([](const NonCopyable& nc) { return nc.GetValue() % 2 == 0; });
        
    for (auto&& nc : list) {
        std::cout << nc.GetValue() << ' ';
    }
    std::cout << std::endl;
}

int main() {
    Sample1();
    Sample2();
    return 0;
}
