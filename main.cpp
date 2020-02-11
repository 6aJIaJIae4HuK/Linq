#include "linq.h"
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v { 2, 3, 4, 5, 6 };
    auto c = AsCollection(v).Filter([](int i) { return i % 2 == 0; });
    for (int i : c) {
        std::cout << i << ' ';
    }
    return 0;
}
