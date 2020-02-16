#include "include/linq.h"
#include <vector>
#include <iostream>
#include <list>
#include <string>

int main() {
    std::vector<std::string> v{ "2", "22", "222" };
    const auto& list = AsCollection(v)
        .Filter([](auto i) { return true; })
        .Reverse();
    auto it = list.begin();
    it++;
    std::cout << it->length() << std::endl;
    for (auto i : Range(static_cast<size_t>(5)).Reverse()) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    return 0;
}
