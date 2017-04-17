#ifndef CHUNKEDBUFFER_H
#define CHUNKEDBUFFER_H

#include "fast_vector.h"
#include "../Type.h"

namespace grynca {

    class ChunkedBuffer {
        static constexpr u32 target_chunk_bytes_ = 65536;
    public:
        explicit ChunkedBuffer()
         : item_size_(1), chunk_size_(target_chunk_bytes_), size_(0), capacity_(0)
        {
        }

        explicit ChunkedBuffer(u32 item_size)
         : item_size_(item_size), chunk_size_(calcChunkSize_(item_size)), size_(0), capacity_(0)
        {
        }

        ~ChunkedBuffer() {
            for (u32 i=0; i<blocks_.size(); ++i) {
                delete[] blocks_[i];
            }
        }

        void init(u32 item_size) {
            ASSERT(empty());
            item_size_ = item_size;
            chunk_size_ = calcChunkSize_(item_size);
        }

        u32 getSize()const { return size_; }
        u32 getCapacity()const { return capacity_; }
        u32 getChunksCount()const { return u32(blocks_.size()); }
        const u8* getChunkData(u32 chunk_id)const {
            return blocks_[chunk_id];
        }

        void resize(u32 n) {
            reserve(n);
            size_ = n;
        }

        void reserve(size_t n) {
            while (capacity_ < n) {
                u8* chunk = new u8[item_size_*chunk_size_];
                blocks_.push_back(chunk);
                capacity_ += chunk_size_;
            }
        }

        u8* accItem(std::size_t n) {
            ASSERT(n < size_);
            return blocks_[n / chunk_size_] + (n % chunk_size_) * item_size_;
        }

        const u8* getItem(std::size_t n) const {
            ASSERT(n < size_);
            return blocks_[n / chunk_size_] + (n % chunk_size_) * item_size_;
        }

        void clear() {
            size_ = 0;
        }

        bool empty()const {
            return size_ == 0;
        }

    private:

        static u32 calcChunkSize_(u32 item_size) {
            u32 items_in_chunk = target_chunk_bytes_ / item_size;
            u32 prev_pt = calcNextPowerOfTwo(items_in_chunk);
            u32 next_pt = calcPrevPowerOfTwo(items_in_chunk);
            if ( (items_in_chunk - prev_pt) < (next_pt - items_in_chunk) )
                return prev_pt;
            return next_pt;
        }

        fast_vector<u8*> blocks_;
        u32 item_size_;
        u32 chunk_size_;
        u32 size_;
        u32 capacity_;
    };

}

#endif //CHUNKEDBUFFER_H
