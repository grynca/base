#ifndef ITEMS2_H
#define ITEMS2_H

#include "fast_vector.h"
#include "../Type.h"

namespace grynca {

    // facade for bytes array

    class Items2 {
    public:
        Items2(fast_vector<u8>& bytes, const TypeInfo& ti)
         : bytes_(&bytes), ti_(ti)
        {
            ASSERT( bytes_->size()%ti.getSize() == 0 );
        }

        u32 size() {
            return u32(bytes_->size()/ti_.getSize());
        }

        bool empty() {
            return bytes_->empty();
        }

        // calls default constructor
        u8* push_back() {
            u32 pos = u32(bytes_->size());
            bytes_->resize(pos+ti_.getSize());
            u8* ptr = getBytePtr_(pos);
            ti_.getDefaultConstr()(ptr);
            return ptr;
        }

        // calls default constructor
        u8* insert(u32 id) {
            u32 pos = u32(id*ti_.getSize());
            u32 tail_size = u32(bytes_->size() - pos);
            bytes_->resize(bytes_->size()+ti_.getSize());

            u8* from = &(*bytes_)[pos];
            u8* to = from+ti_.getSize();

            memmove(to, from, tail_size);
            ti_.getDefaultConstr()(from);
            return from;
        }

        // caller must construct himself
        u8* push_back2() {
            u32 pos = u32(bytes_->size());
            bytes_->resize(pos+ti_.getSize());
            u8* ptr = getBytePtr_(pos);
            return ptr;
        }

        // caller must construct himself
        u8* insert2(u32 id) {
            u32 pos = u32(id*ti_.getSize());
            u32 tail_size = u32(bytes_->size() - pos);
            bytes_->resize(bytes_->size()+ti_.getSize());

            u8* from = &(*bytes_)[pos];
            u8* to = from+ti_.getSize();

            memmove(to, from, tail_size);
            return from;
        }

        void remove(u32 id) {
            u32 pos_from = u32(id*ti_.getSize());
            u32 pos_to = u32(pos_from + ti_.getSize());
            u8* ptr = getBytePtr_(pos_from);
            ti_.getDestroyFunc()(ptr);
            bytes_->erase(bytes_->begin()+pos_from, bytes_->begin()+pos_to);
        }

        void remove(u32 id, u32 cnt) {
            u32 pos_from = u32(id*ti_.getSize());
            u32 pos_to = pos_from;
            for (u32 i=0; i<cnt; ++i, pos_to+=ti_.getSize()) {
                u8* ptr = getBytePtr_(pos_to);
                ti_.getDestroyFunc()(ptr);
            }

            bytes_->erase(bytes_->begin()+pos_from, bytes_->begin()+pos_to);
        }

        void pop_back() {
            ASSERT(!bytes_->empty());
            u32 pos = u32(bytes_->size()-ti_.getSize());
            u8* ptr = getBytePtr_(pos);
            ti_.getDestroyFunc()(ptr);
            bytes_->erase(bytes_->begin()+pos, bytes_->end());
        }

        u8* replaceWithLast(u32 id) {
            u8* last_ptr = back();
            u8* item_ptr = getItemPtr(id);
            ti_.getCopyFunc()(item_ptr, last_ptr);
            pop_back();
            return item_ptr;
        }

        u8* getItemPtr(u32 id) {
            u32 pos = u32(id*ti_.getSize());
            return getBytePtr_(pos);
        }

        u8* back() {
            ASSERT(!bytes_->empty());
            u32 pos = u32(bytes_->size()-ti_.getSize());
            return getBytePtr_(pos);
        }

        u8* front() {
            ASSERT(!bytes_->empty());
            return getBytePtr_(0);
        }
    private:

        u8* getBytePtr_(u32 byte_pos) {
            return &(*bytes_)[byte_pos];
        }

        fast_vector<u8>* bytes_;
        const TypeInfo& ti_;
    };

}

#endif //ITEMS2_H
