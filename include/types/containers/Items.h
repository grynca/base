#ifndef ITEMS_H
#define ITEMS_H

#include "fast_vector.h"
#include "../Type.h"

namespace grynca {

    // facade for bytes array

    template <typename ItemType>
    class Items {
    public:
        Items(fast_vector<u8>& bytes)
         : bytes_(&bytes)
        {
            ASSERT( bytes_->size()%sizeof(ItemType) == 0 );
        }

        u32 size() {
            return u32(bytes_->size()/sizeof(ItemType));
        }

        bool empty() {
            return bytes_->empty();
        }

        template <typename ... ConstrArgs>
        ItemType& emplace_back(ConstrArgs&&... args) {
            u32 pos = bytes_->size();
            bytes_->resize(pos+sizeof(ItemType));
            return *Type<ItemType>::create(getBytePtr_(pos), std::forward<ConstrArgs>(args)...);
        }

        template <typename ... ConstrArgs>
        ItemType& emplace(u32 id, ConstrArgs&&... args) {
            u32 pos = id*sizeof(ItemType);
            u32 tail_size = bytes_->size() - pos;
            bytes_->resize(bytes_->size()+sizeof(ItemType));

            u8* from = &(*bytes_)[pos];
            u8* to = from+sizeof(ItemType);

            memmove(to, from, tail_size);
            return *Type<ItemType>::create(from, std::forward<ConstrArgs>(args)...);
        }

        void remove(u32 id) {
            u32 pos_from = id*sizeof(ItemType);
            u32 pos_to = pos_from + sizeof(ItemType);

            Type<ItemType>::destroy(getBytePtr_(pos_from));
            bytes_->erase(bytes_->begin()+pos_from, bytes_->begin()+pos_to);
        }

        void remove(u32 id, u32 cnt) {
            u32 pos_from = id*sizeof(ItemType);
            u32 pos_to = pos_from;
            for (u32 i=0; i<cnt; ++i, pos_to+=sizeof(ItemType)) {
                Type<ItemType>::destroy(getBytePtr_(pos_to));
            }

            bytes_->erase(bytes_->begin()+pos_from, bytes_->begin()+pos_to);
        }

        void pop_back() {
            ASSERT(!bytes_->empty());
            u32 pos = bytes_->size()-sizeof(ItemType);
            Type<ItemType>::destroy(getBytePtr_(pos));
            bytes_->erase(bytes_->begin()+pos, bytes_->end());
        }

        ItemType& replaceWithLast(u32 id) {
            ItemType& last = back();
            ItemType& item = operator[](id);
            item = last;
            pop_back();
            return item;
        }

        ItemType& operator[](u32 id) {
            return (ItemType&)(*bytes_)[id*sizeof(ItemType)];
        }

        ItemType& back() {
            ASSERT(!bytes_->empty());
            u32 pos = bytes_->size()-sizeof(ItemType);
            return (ItemType&)(*bytes_)[pos];
        }

        ItemType& front() {
            ASSERT(!bytes_->empty());
            return (ItemType&)(*bytes_)[0];
        }

        u8* getDataPtr() {
            ASSERT(!bytes_->empty());
            return getBytePtr_(0);
        }
    private:

        u8* getBytePtr_(u32 byte_pos) {
            return &(*bytes_)[byte_pos];
        }

        fast_vector<u8>* bytes_;
    };

}

#endif //ITEMS_H
