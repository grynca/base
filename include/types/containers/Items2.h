#ifndef ITEMS2_H
#define ITEMS2_H

#include "fast_vector.h"
#include "../Type.h"

namespace grynca {

    // facade for bytes array

    template <typename BytesType = fast_vector<u8> >
    class Items2 {
    public:
        typedef u32 IndexType;

        Items2() : ti_(NULL) {}
        Items2(const TypeInfo& ti) : ti_(&ti) {}

        template <typename ItemType>
        void init() {
            ti_ = &Type<ItemType>::getInternalTypeInfo();
        }

        void init(const TypeInfo& ti) {
            ti_ = &ti;
        }

        u32 size() {
            return u32(bytes_.size()/ti_->getSize());
        }

        bool empty() {
            return bytes_.empty();
        }

        // calls default constructor
        u8* emplace_back() {
            u32 pos = u32(bytes_.size());
            bytes_.enlarge(pos+ti_->getSize());
            u8* ptr = accBytePtr_(pos);
            ti_->getDefaultConstr()(ptr);
            return ptr;
        }

        // calls default constructor
        u8* insert(u32 id) {
            u32 pos = u32(id*ti_->getSize());
            u32 tail_size = u32(bytes_.size() - pos);
            bytes_.enlarge(bytes_.size()+ti_->getSize());

            u8* from = &(*bytes_)[pos];
            u8* to = from+ti_->getSize();

            memmove(to, from, tail_size);
            ti_->getDefaultConstr()(from);
            return from;
        }

        // caller must construct himself
        u8* push_back2() {
            u32 pos = u32(bytes_.size());
            bytes_.enlarge(pos+ti_->getSize());
            u8* ptr = accBytePtr_(pos);
            return ptr;
        }

        // caller must construct himself
        u8* insert2(u32 id) {
            u32 pos = u32(id*ti_->getSize());
            u32 tail_size = u32(bytes_.size() - pos);
            bytes_.enlarge(bytes_.size()+ti_->getSize());

            u8* from = &(*bytes_)[pos];
            u8* to = from+ti_->getSize();

            memmove(to, from, tail_size);
            return from;
        }

        void reserve(u32 cnt) {
            bytes_.reserve(ti_->getSize()*cnt);
        }

        void removeItem(u32 id) {
            u32 pos_from = u32(id*ti_->getSize());
            u32 pos_to = u32(pos_from + ti_->getSize());
            u8* ptr = accBytePtr_(pos_from);
            ti_->getDestroyFunc()(ptr);
            bytes_.erase(bytes_.begin()+pos_from, bytes_.begin()+pos_to);
        }

        void removeItems(u32 id, u32 cnt) {
            u32 pos_from = u32(id*ti_->getSize());
            u32 pos_to = pos_from;
            for (u32 i=0; i<cnt; ++i, pos_to+=ti_->getSize()) {
                u8* ptr = accBytePtr_(pos_to);
                ti_->getDestroyFunc()(ptr);
            }

            bytes_.erase(bytes_.begin()+pos_from, bytes_.begin()+pos_to);
        }

        void pop_back() {
            ASSERT(!bytes_.empty());
            u32 pos = u32(bytes_.size()-ti_->getSize());
            u8* ptr = accBytePtr_(pos);
            ti_->getDestroyFunc()(ptr);
            bytes_.erase(bytes_.begin()+pos, bytes_.end());
        }

        u8* replaceWithLast(u32 id) {
            u8* last_ptr = back();
            u8* item_ptr = accItemPtr(id);
            ti_->getCopyFunc()(item_ptr, last_ptr);
            pop_back();
            return item_ptr;
        }

        u8* accItemPtr(u32 id) {
            u32 pos = u32(id*ti_->getSize());
            return accBytePtr_(pos);
        }

        template <typename ItemType>
        ItemType& accItem(u32 id) {
            ASSERT(Type<ItemType>::getInternalTypeId() == ti_->getId());
            u32 pos = u32(id*ti_->getSize());
            return *(ItemType*)accBytePtr_(pos);
        }

        const u8* getItemPtr(u32 id)const {
            u32 pos = u32(id*ti_->getSize());
            return getBytePtr_(pos);
        }

        template <typename ItemType>
        const ItemType& getItem(u32 id)const {
            ASSERT(Type<ItemType>::getInternalTypeId() == ti_->getId());
            u32 pos = u32(id*ti_->getSize());
            return *(ItemType*)getBytePtr_(pos);
        }

        u8* back() {
            ASSERT(!bytes_.empty());
            u32 pos = u32(bytes_.size()-ti_->getSize());
            return accBytePtr_(pos);
        }

        u8* front() {
            ASSERT(!bytes_.empty());
            return accBytePtr_(0);
        }

        const TypeInfo& getTypeInfo()const {
            ASSERT(ti_);
            return *ti_;
        }

        const BytesType& getInnerContainer()const {
            return bytes_;
        }

        BytesType& accInnerContainer() {
            return bytes_;
        }
        
    protected:
        Items2(BytesType bytes, const TypeInfo& ti)
         : bytes_(bytes), ti_(&ti)
        {
            ASSERT( bytes_.size()%ti.getSize() == 0 );
        }
        
        u8* accBytePtr_(u32 byte_pos) {
            return &bytes_[byte_pos];
        }

        const u8* getBytePtr_(u32 byte_pos)const {
            return &bytes_[byte_pos];
        }

        BytesType bytes_;
        const TypeInfo* ti_;
    };


    template <typename BytesType = fast_vector<u8> >
    class Items2Ref : public Items2<BytesType&> {
    public:
        Items2Ref(BytesType& bytes, const TypeInfo& ti) : Items2<BytesType&>(bytes, ti) {}
    };
}

#endif //ITEMS2_H
