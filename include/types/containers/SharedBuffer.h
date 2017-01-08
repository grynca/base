#ifndef SHAREDBUFFER_H
#define SHAREDBUFFER_H

#include "../../functions/defs.h"
#include <atomic>

namespace grynca {

    class SharedBuffer {
    public:
        SharedBuffer();     // NULL
        SharedBuffer(u32 max_size);
        SharedBuffer(void* data, u32 data_size);      // wraps around already allocated buffer (no refcounting)
        SharedBuffer& operator=(const SharedBuffer& buf);
        SharedBuffer(const SharedBuffer& buf);
        virtual ~SharedBuffer();

        SharedBuffer clone()const;

        bool isNull();
        void setSize(u32 s);
        u32 getSize()const;
        u32 getAllocatedSize()const;

        const u8* getData()const;
        u8* getData();
        u8* getBack(); //  getData()[getSize()]
                            //  when used for storing data after back, changed size must be set via setSize()
        void pushBack(const void* buf, u32 buf_size);
        void insert(u32 id, const void* buf, u32 buf_size);
        bool isEmpty()const;
        void clear();

    private:
        u8* data_;
        u32 size_;
        u32 allocated_;
        std::atomic<u32>* ref_count_;
    };

}

#include "SharedBuffer.inl"
#endif //SHAREDBUFFER_H
