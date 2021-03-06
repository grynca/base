//
// Created by lutza on 14.7.2015.
//

#ifndef RANDOM_H
#define RANDOM_H

#include "../types/containers/fast_vector.h"
#include <cassert>

namespace grynca {

    // 0-1
    static inline f32 randFloat() {
        return f32(rand()%RAND_MAX)/RAND_MAX;
    }

    // 0-to
    static inline f32 randFloat(f32 to) {
        return randFloat()*to;
    }

    // from-to
    static inline f32 randFloat(f32 from, f32 to) {
        return randFloat()*(to-from) + from;
    }

    // -1 or 1
    static inline f32 randSign() {
        return (rand()%2)*2 - 1;
    }

    // pick count numbers without repeating, from 0 to (max-1), caller is responsible for seeding his random gen
    static inline void randomPickN(fast_vector<unsigned int>& picked, unsigned int count, unsigned int max) {
        ASSERT_M(count <= max, "Cant pick more numbers than max.");

        // create vector of numbers from 0..(max-1)
        fast_vector<unsigned int> numbers_pool;
        numbers_pool.reserve(max);
        for (unsigned int i=0; i<max; i++)
            numbers_pool.push_back(i);

        unsigned int already_picked = 0;
        while (already_picked<count) {
            unsigned int rn = rand()%(max-already_picked);
            // swap picked element to start of pool
            if (rn!=0) {
                unsigned int tmp = numbers_pool[already_picked];
                numbers_pool[already_picked] = numbers_pool[already_picked+rn];
                numbers_pool[already_picked+rn] = tmp;
            }
            already_picked++;
        }

        // our picked elements are at the beginning of numbers pool
        // move them to output vector
        for (unsigned int i=0; i< already_picked; i++)
            picked.push_back(numbers_pool[i]);
    }
}

#endif //RANDOM_H
