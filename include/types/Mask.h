#ifndef MASK_H
#define MASK_H

#include "../functions/debug.h"
#include <bitset>
#include <stdint.h>

namespace grynca {

    template <int S>
    class Mask : public std::bitset<S> {
        typedef std::bitset<S> Parent;
    public:
        template <typename BS>
        Mask(const BS& m) : Parent(m.to_ulong()) { ASSERT(m.size() < 64); }

        constexpr Mask() : Parent(0) {}
        Mask(Parent&& p) : Parent(p) {}

        Mask(const std::initializer_list<uint32_t> il) {
            for (auto it = il.begin(); it!=il.end(); ++it) {
                Parent::set(*it);
            }
        }

        static constexpr size_t size() { return (size_t)S; }

        // returns index of first 1 bit or size() if not found
        uint32_t findFirst() {
            return Parent::_Find_first();
        }

        // returns index of first 1 bit after prev or size() if not found
        uint32_t findNext(uint32_t prev) {
            return Parent::_Find_next(prev);
        }

        bool anySet(const Mask<S>& m)const {
            return ((*this)&m).any();
        }

        bool allSet(const Mask<S>& m)const {
            return ((*this)&m) == m;
        }
    };
}

#endif //MASK_H
