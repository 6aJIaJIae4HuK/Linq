#include "linq.h"
#include <vector>
#include <iostream>
#include <list>

int main() {
    std::vector<int> v { 2, 3, 4, 5, 6 };
    const auto& list = AsCollection(v)
        .Filter([](int i) { return i % 2 == 0; })
        .Reverse()
        .To<std::list<int>>();
    for (int i : list) {
        std::cout << i << ' ';
    }
    return 0;
}
