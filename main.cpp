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
    for (auto i : Range(-4, 11, 3)) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    return 0;
}
