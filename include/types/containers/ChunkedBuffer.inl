#include "ChunkedBuffer.h"

namespace grynca {

    inline ChunkedBuffer::ChunkedBuffer()
     : item_size_(1), chunk_size_(target_chunk_bytes_), size_(0), capacity_(0)
    {}

    inline ChunkedBuffer::ChunkedBuffer(u32 item_size)
     : item_size_(item_size), chunk_size_(calcChunkSize_(item_size)), size_(0), capacity_(0)
    {}

    inline ChunkedBuffer::~ChunkedBuffer() {
        for (u32 i=0; i<blocks_.size(); ++i) {
            delete[] blocks_[i];
        }
    }

    inline void ChunkedBuffer::init(u32 item_size) {
        ASSERT(isEmpty());
        item_size_ = item_size;
        chunk_size_ = calcChunkSize_(item_size);
    }

    inline u32 ChunkedBuffer::getItemSize()const {
        return item_size_;
    }

    inline u32 ChunkedBuffer::getSize()const {
        return size_;
    }

    inline u32 ChunkedBuffer::getCapacity()const {
        return capacity_;
    }

    inline u32 ChunkedBuffer::getChunksCount()const {
        return u32(blocks_.size());
    }

    inline u32 ChunkedBuffer::getOccupiedChunksCount()const {
        return size_/chunk_size_ + ((size_%chunk_size_)!=0);
    }

    inline u32 ChunkedBuffer::getChunkSize()const {
        return chunk_size_;
    }

    inline const u8* ChunkedBuffer::getChunkData(u32 chunk_id)const {
        return blocks_[chunk_id];
    }

    inline void ChunkedBuffer::resize(u32 n) {
        reserve(n);
        size_ = n;
    }

    inline void ChunkedBuffer::reserve(u32 n) {
        while (capacity_ < n) {
            u8* chunk = new u8[item_size_*chunk_size_];
            blocks_.push_back(chunk);
            capacity_ += chunk_size_;
        }
    }

    inline u8* ChunkedBuffer::accItem(u32 item_id) {
        ASSERT(item_id < size_);
        return blocks_[item_id / chunk_size_] + (item_id % chunk_size_) * item_size_;
    }

    inline const u8* ChunkedBuffer::getItem(u32 item_id) const {
        ASSERT(item_id < size_);
        return blocks_[item_id / chunk_size_] + (item_id % chunk_size_) * item_size_;
    }

    template <typename LoopFunc>
    inline void ChunkedBuffer::forEach(const LoopFunc& lf)const {
        u32 last_chunk_id = size_/chunk_size_;
        u32 i = 0;
        for (; i<last_chunk_id; ++i) {
            for (u32 j=0; j<chunk_size_; ++j) {
                lf(blocks_[i] + j*item_size_);
            }
        }
        u32 last_chunk_size = size_%chunk_size_;
        for (u32 j=0; j<last_chunk_size; ++j) {
            lf(blocks_[i] + j*item_size_);
        }
    }

    template <typename LoopFunc>
    inline void ChunkedBuffer::forEachWithId(const LoopFunc& lf)const {
        u32 last_chunk_id = size_/chunk_size_;
        u32 i = 0;
        for (; i<last_chunk_id; ++i) {
            for (u32 j=0; j<chunk_size_; ++j) {
                u32 item_id = i*chunk_size_+j;
                lf(blocks_[i] + j*item_size_, item_id);
            }
        }
        u32 last_chunk_size = size_%chunk_size_;
        for (u32 j=0; j<last_chunk_size; ++j) {
            u32 item_id = i*chunk_size_+j;
            lf(blocks_[i] + j*item_size_, item_id);
        }
    }

    inline void ChunkedBuffer::clear() {
        size_ = 0;
    }

    inline bool ChunkedBuffer::isEmpty()const {
        return size_ == 0;
    }

    inline u32 ChunkedBuffer::calcChunkSize_(u32 item_size) {
        // static
        u32 items_in_chunk = target_chunk_bytes_ / item_size;
        u32 prev_pt = calcNextPowerOfTwo(items_in_chunk);
        u32 next_pt = calcPrevPowerOfTwo(items_in_chunk);
        if ( (items_in_chunk - prev_pt) < (next_pt - items_in_chunk) )
            return prev_pt;
        return next_pt;
    }
}