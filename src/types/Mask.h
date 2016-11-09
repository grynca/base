#ifndef MASK_H
#define MASK_H

#include "../functions/defs.h"
#include <bitset>

namespace grynca {

    template <int S>
    class Mask : public std::bitset<S> {
        typedef std::bitset<S> Parent;
    public:
        constexpr Mask() : Parent(0) {}
        Mask(Parent&& p) : Parent(p) {}

        Mask(const std::initializer_list<u32> il) {
            for (auto it = il.begin(); it!=il.end(); ++it) {
                Parent::set(*it);
            }
        }

        static constexpr size_t size() { return (size_t)S; }

        bool anySet(const Mask<S>& m)const {
            return ((*this)&m).any();
        }

        bool allSet(const Mask<S>& m)const {
            return ((*this)&m) == m;
        }
    };
}

#endif //MASK_H
