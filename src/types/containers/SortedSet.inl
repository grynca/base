#include "SortedSet.h"
#include "../../functions/sort_utils.h"

#define SS_TPL template <typename ItemType>
#define SS_TYPE SortedSet<ItemType>

namespace grynca {

    SS_TPL
    template <typename ... ConstrArgs>
    inline ItemType& SS_TYPE::add(uint32_t id, ConstrArgs&& ... args) {
        uint32_t pos = bisectFindInsert(ids_, id);
        ASSERT_M(pos >= ids_.size() || ids_[pos] != id, "already contained.");

        if (positions_.size() <= id) {
            positions_.resize(id+1, InvalidId());
        }
        positions_[id] = pos;
        ids_.insert(ids_.begin()+pos, id);
        for (uint32_t i=pos+1; i<ids_.size(); ++i) {
            ++positions_[ids_[i]];
        }
        return *items_.emplace(items_.begin()+pos, std::forward<ConstrArgs>(args)...);
    }

    SS_TPL
    inline ItemType& SS_TYPE::get(uint32_t id) {
        ASSERT(isContained(id));
        return items_[positions_[id]];
    }

    SS_TPL
    inline bool SS_TYPE::isContained(uint32_t id)const {
        return (id < positions_.size() && positions_[id]!=InvalidId());
    }

    SS_TPL
    inline const ItemType& SS_TYPE::get(uint32_t id)const {
        ASSERT(isContained(id));
        return items_[positions_[id]];
    }

    SS_TPL
    inline ItemType& SS_TYPE::getAtPos(uint32_t pos) {
        return items_[pos];
    }

    SS_TPL
    inline const ItemType& SS_TYPE::getAtPos(uint32_t pos)const {
        return items_[pos];
    }

    SS_TPL
    inline void SS_TYPE::remove(uint32_t id) {
        uint32_t pos = positions_[id];
        ids_.erase(ids_.begin()+pos);
        items_.erase(items_.begin()+pos);
        positions_[id] = InvalidId();
        for (uint32_t i=pos; i<ids_.size(); ++i) {
            --positions_[ids_[i]];
        }
    }

    SS_TPL
    inline void SS_TYPE::removeAtPos(uint32_t pos) {
        uint32_t id = ids_[pos];
        ids_.erase(ids_.begin()+pos);
        items_.erase(items_.begin()+pos);
        positions_[id] = InvalidId();
        for (uint32_t i=pos; i<ids_.size(); ++i) {
            --positions_[ids_[i]];
        }
    }

    SS_TPL
    inline uint32_t SS_TYPE::getIndexForPos(uint32_t pos)const {
        return ids_[pos];
    }

    SS_TPL
    inline uint32_t SS_TYPE::getPosForIndex(uint32_t id)const {
        return positions_[id];
    }

    SS_TPL
    inline uint32_t SS_TYPE::size()const {
        return items_.size();
    }

    SS_TPL
    inline void SS_TYPE::reserve(uint32_t size) {
        positions_.resize(size, InvalidId());
        ids_.reserve(size);
        items_.reserve(size);
    }

    SS_TPL
    inline bool SS_TYPE::empty() {
        return items_.empty();
    }

    SS_TPL
    inline void SS_TYPE::clear() {
        positions_.clear();
        items_.clear();
        ids_.clear();
    }

}

#undef SS_TPL
#undef SS_TYPE