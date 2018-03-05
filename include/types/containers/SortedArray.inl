#include "SortedArray.h"
#include "types/Index.h"
#include "functions/sort_utils.h"

#define FREE_BITMASK (1<<31)
#define SA_TPL template <typename ItemType>
#define SA_TYPE SortedArray<ItemType>

namespace grynca {

    SA_TPL
    inline SA_TYPE::SortedArray()
     : first_free_id_(InvalidId())
    {}

    SA_TPL
    inline void SA_TYPE::reserve(u32 count) {
        items_.reserve(count);
        id_to_pos_.reserve(count);
    }

    SA_TPL
    inline ItemType& SA_TYPE::addLast(const ItemType& item) {
        u32 id = getNextId_();
        id_to_pos_[id] = u32(items_.size());
        items_.push_back(item);
        ItemType& rslt = items_.back();
        rslt.accId_() = id;
        return rslt;
    }

    SA_TPL
    inline ItemType& SA_TYPE::add(const ItemType& item) {
        u32 id = getNextId_();
        // find pos
        u32 pos = bisectFindInsert(items_, item, typename ItemType::Comparator());
        id_to_pos_[id] = pos;
        ItemType& rslt = *items_.insert(items_.begin()+pos, item);
        rslt.accId_() = id;

        // update positions of items after inserted
        for (u32 i=pos+1; i<u32(items_.size()); ++i) {
            ++id_to_pos_[items_[i].accId_()];
        }
        return rslt;
    }

    SA_TPL
    inline void SA_TYPE::removeItem(u32 item_id) {
        ASSERT(containsItem(item_id));
        u32 pos = id_to_pos_[item_id];
        // update positions of items after inserted
        for (u32 i=pos+1; i<u32(items_.size()); ++i) {
            --id_to_pos_[items_[i].accId_()];
        }
        items_.erase(items_.begin()+pos);
        // add to list of freed
        id_to_pos_[item_id] = FREE_BITMASK | first_free_id_;
        first_free_id_ = item_id;
    }

    SA_TPL
    inline const ItemType& SA_TYPE::getItem(u32 item_id)const {
        const_cast<SA_TYPE*>(this)->accItem(item_id);
    }

    SA_TPL
    inline const ItemType* SA_TYPE::tryGetItem(u32 item_id)const {
        return const_cast<SA_TYPE*>(this)->tryAccItem(item_id);
    }

    SA_TPL
    inline ItemType& SA_TYPE::accItem(u32 item_id) {
        ASSERT(containsItem(item_id));
        u32 pos = id_to_pos_[item_id];
        return items_[pos];
    }

    SA_TPL
    inline ItemType* SA_TYPE::tryAccItem(u32 item_id) {
        if (!containsItem(item_id))
            return NULL;
        u32 pos = id_to_pos_[item_id];
        return &items_[pos];
    }

    SA_TPL
    inline typename SA_TYPE::IRefPtr SA_TYPE::getRef(u32 item_id) {
        return IRefPtr(*this, item_id);
    }

    SA_TPL
    inline bool SA_TYPE::containsItem(u32 item_id)const {
        return item_id <= id_to_pos_.size() && !isFree_(item_id);
    }

    SA_TPL
    inline const ItemType& SA_TYPE::getItemAtPos(u32 pos)const {
        return items_[pos];
    }

    SA_TPL
    inline ItemType& SA_TYPE::accItemAtPos(u32 pos) {
        return items_[pos];
    }

    SA_TPL
    inline u32 SA_TYPE::getItemPos(u32 item_id)const {
        return id_to_pos_[item_id];
    }

    SA_TPL
    inline u32 SA_TYPE::size()const {
        return u32(items_.size());
    }

    SA_TPL
    inline bool SA_TYPE::empty()const {
        return items_.empty();
    }

    SA_TPL
    inline void SA_TYPE::clear() {
        items_.clear();
        id_to_pos_.clear();
    }

    SA_TPL
    template <typename LoopFunc>
    inline void SA_TYPE::loopItems(const LoopFunc& lf)const {
        for (u32 i=0; u32(i<items_.size()); ++i) {
            lf(items_[i]);
        }
    }

    SA_TPL
    template <typename LoopFunc>
    void SA_TYPE::loopItemsBreakable(const LoopFunc& lf)const {
        for (u32 i=0; i<u32(items_.size()); ++i) {
            if (lf(items_[i])) {
                break;
            }
        }
    }

    SA_TPL
    inline bool SA_TYPE::isFree_(u32 id)const {
        return (id_to_pos_[id] & FREE_BITMASK) != 0;
    }

    SA_TPL
    inline u32 SA_TYPE::getNextId_() {
        u32 id;
        if (first_free_id_ == InvalidId()) {
            id = u32(id_to_pos_.size());
            id_to_pos_.push_back();
        }
        else {
            id = first_free_id_;
            u32 next_free = id_to_pos_[first_free_id_];
            first_free_id_ = (next_free == InvalidId())
                             ? InvalidId()
                             : next_free &~FREE_BITMASK;
        }
        return id;
    }

}

#undef SA_TPL
#undef SA_TYPE
#undef FREE_BITMASK