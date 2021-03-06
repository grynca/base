#include "SharedBuffer.h"
#include <stddef.h>
#include <assert.h>
#include <string.h>

namespace grynca {

    inline SharedBuffer::SharedBuffer()
     : data_(NULL),
       size_(0),
       allocated_(0),
       ref_count_(NULL)
    {}

    inline SharedBuffer::SharedBuffer(u32 max_size)
     : data_(new u8[max_size]),
       size_(0),
       allocated_(max_size),
       ref_count_(new std::atomic<u32>(1))
    {}

    inline SharedBuffer::SharedBuffer(void* data, u32 data_size)
     : data_((u8*)data),
       size_(data_size),
       allocated_(0),
       ref_count_(NULL)
    {}

    inline SharedBuffer::SharedBuffer(const SharedBuffer& buf)
    : data_(buf.data_),
      size_(buf.size_),
      allocated_(buf.allocated_),
      ref_count_(buf.ref_count_)
    {
        if (ref_count_)
            ++*ref_count_;
    }

    inline SharedBuffer::~SharedBuffer() {
        if (ref_count_) {
            if (--*ref_count_ == 0) {
                delete ref_count_;
                delete[] data_;
            }
        }
    }

    inline SharedBuffer& SharedBuffer::operator=(const SharedBuffer& buf) {
        data_ = buf.data_;
        size_ = buf.size_;
        allocated_ = buf.allocated_;

        ref_count_ = buf.ref_count_;
        if (ref_count_) {
            ++*ref_count_;
        }
        return *this;
    }

    inline SharedBuffer SharedBuffer::clone() const {
        if (allocated_) {
            SharedBuffer buf2(allocated_);
            buf2.pushBack(data_, size_);
            return buf2;
        }

        return SharedBuffer(data_, size_);
    }

    inline bool SharedBuffer::isNull() {
        return data_==NULL;
    }

    inline void SharedBuffer::setSize(u32 s) {
        size_ = s;
    }

    inline u32 SharedBuffer::getSize()const {
        return size_;
    }

    inline u32 SharedBuffer::getAllocatedSize()const {
        return allocated_;
    }

    inline const u8* SharedBuffer::getData()const {
        return data_;
    }

    inline u8* SharedBuffer::getData() {
        return data_;
    }

    inline u8* SharedBuffer::getBack() {
        return &data_[size_];
    }

    inline void SharedBuffer::pushBack(const void* buf, u32 buf_size) {
        ASSERT(size_+buf_size <= allocated_);
        memcpy(&data_[size_], buf, buf_size);
        size_ += buf_size;
    }


    inline void SharedBuffer::insert(u32 id, const void* buf, u32 buf_size) {
        ASSERT(size_+buf_size <= allocated_);
        if (id >= size_) {
            pushBack(buf, buf_size);
        }
        else {
            memmove(&data_[id+buf_size], &data_[id], size_-id);
            memcpy(&data_[id], buf, buf_size);
            size_ += buf_size;
        }
    }

    inline bool SharedBuffer::isEmpty()const {
        return size_ == 0;
    }

    inline void SharedBuffer::clear() {
        size_ = 0;
    }

}