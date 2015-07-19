#include <iostream>
#include "src/base.h"
#include <unordered_map>
using namespace std;
using namespace grynca;

class MyStuff {
public:
    MyStuff() {
    }
    ~MyStuff() { }

    void init() {
        a = rand()%10;
    }

    void dostuff() {
        a +=rand()%10;
    }

    int a;
};

int main(int argc, char** argv) {
    size_t n = 1e7;
    std::unordered_map<uint32_t, MyStuff> stuff_map;
    stuff_map.reserve(n);
    UnsortedVector<MyStuff> stuff_vec;
    stuff_vec.reserve(n);
    fast_vector<unsigned int> destruction_order;
    destruction_order.reserve(n);
    randomPickN(destruction_order, n, n);

    {
        BlockMeasure m("MAP creation");
        for (size_t i=0; i<n; ++i) {
            stuff_map[i] = MyStuff();
        }
    }

    {
        BlockMeasure m("MAP loop");
        for (size_t i=0; i<n; ++i) {
            stuff_map[i].dostuff();
        }
    }

    {
        BlockMeasure m("MAP destruction");
        for (size_t i=0; i<n; ++i) {
            stuff_map.erase(destruction_order[i]);
        }
    }

    {
        BlockMeasure m("UNSORTED_VECTOR creation");
        for (size_t i=0; i<n; ++i) {
            stuff_vec.add();
        }
    }

    {
        BlockMeasure m("UNSORTED_VECTOR loop");
        for (size_t i=0; i<n; ++i) {
            stuff_vec.get(i).dostuff();
        }
    }

    {
        BlockMeasure m("UNSORTED_VECTOR destruction");
        for (size_t i=0; i<n; ++i) {
            stuff_vec.remove(destruction_order[i]);
        }
    }


    KEY_TO_CONTINUE();
    return 0;
}
