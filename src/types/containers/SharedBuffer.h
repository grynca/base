#ifndef SHAREDBUFFER_H
#define SHAREDBUFFER_H

#include <stdint.h>
#include <atomic>

namespace grynca {

    class SharedBuffer {
    public:
        SharedBuffer();     // NULL
        SharedBuffer(uint32_t max_size);
        SharedBuffer(void* data, uint32_t data_size);      // wraps around already allocated buffer (no refcounting)
        SharedBuffer& operator=(const SharedBuffer& buf);
        SharedBuffer(const SharedBuffer& buf);
        virtual ~SharedBuffer();

        SharedBuffer clone()const;

        bool isNull();
        void setSize(uint32_t s);
        uint32_t getSize()const;
        uint32_t getAllocatedSize()const;

        const uint8_t* getData()const;
        uint8_t* getData();
        uint8_t* getBack(); //  getData()[getSize()]
                            //  when used for storing data after back, changed size must be set via setSize()
        void pushBack(const void* buf, uint32_t buf_size);
        void insert(uint32_t id, const void* buf, uint32_t buf_size);
        bool isEmpty()const;
        void clear();

    private:
        uint8_t* data_;
        uint32_t size_;
        uint32_t allocated_;
        std::atomic<uint32_t>* ref_count_;
    };

}

#include "SharedBuffer.inl"
#endif //SHAREDBUFFER_H
