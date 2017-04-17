#ifndef ROUNDBUFFER_H
#define ROUNDBUFFER_H

#include <cassert>
#include "functions/defs.h"

namespace grynca {

    template <typename ItemType, int Size = 32>
    class RoundBuffer {
    public:
        RoundBuffer()
            : curr_id_(0), size_(0)  {}

        void clear() {
            size_ = 0;
        }

        bool isEmpty() {
            return size_ == 0;
        }

        u32 getSize() {
            return size_;
        }

        ItemType& getCurrent() {
            ASSERT(getSize() > 0);
            return items_[curr_id_];
        }

        ItemType& getPrev(u32 prev_offset = 1) {
            ASSERT(((i32)getSize()-prev_offset) > 0);
            return items_[(curr_id_-prev_offset)%Size];
        }

        void add() {
            curr_id_ = (curr_id_+1)%Size;
            ++size_;
        }

    private:
        // round buffer
        ItemType items_[Size];
        i32 curr_id_;
        u32 size_;
    };

}

#endif //ROUNDBUFFER_H
