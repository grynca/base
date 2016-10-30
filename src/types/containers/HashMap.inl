#include "HashMap.h"
#include "../Index.h"
#include "../../functions/common.h"

#define HM_TPL template <typename ItemType, typename KeyType, typename Hasher, typename Compare>
#define HM_TYPE HashMap<ItemType, KeyType, Hasher, Compare>
#define DEF_SIZE 64

namespace grynca {

    HM_TPL
    inline HM_TYPE::HashMap()
     : HashMap(DEF_SIZE)
    {
    }

    HM_TPL
    inline HM_TYPE::HashMap(uint32_t initial_size)
     : items_count_(0), modulo_mask_(initial_size -1), initial_size_(initial_size)
    {
        ASSERT(isPowerOfTwo_(initial_size));
        hash_table_.resize(initial_size, InvalidId());
        data_.resize(initial_size*sizeof(ItemType));
        keys_.resize(initial_size);
#ifdef DEBUG_BUILD
        next_.resize(initial_size, InvalidId());
#else
        next_.resize(initial_size);
#endif
    }

    HM_TPL
    inline void HM_TYPE::reserve(uint32_t size) {
        hash_table_.reserve(size);
        data_.reserve(size*sizeof(ItemType));
        keys_.reserve(size);
        next_.reserve(size);
    }

    HM_TPL
    inline void HM_TYPE::clear() {
        std::fill(hash_table_.begin(), hash_table_.end(), InvalidId());
        std::fill(next_.begin(), next_.end(), InvalidId());
        items_count_ = 0;
    }

    HM_TPL
    inline ItemType* HM_TYPE::addItem(const KeyType& key) {
        uint32_t inner_hash = hash_(key);
        uint32_t hash = inner_hash & modulo_mask_;
        uint32_t id = findInternal_(key, hash);
        if (id != InvalidId())
            // already there
            return getItemPtr_(id);

        return addItemInner_(inner_hash, hash, key);
    }

    HM_TPL
    inline ItemType* HM_TYPE::addItem(const KeyType& key, bool& was_added) {
        uint32_t inner_hash = hash_(key);
        uint32_t hash = inner_hash & modulo_mask_;
        uint32_t id = findInternal_(key, hash);
        if (id != InvalidId()) {
            // already there
            was_added = false;
            return getItemPtr_(id);
        }
        was_added = true;
        return addItemInner_(inner_hash, hash, key);
    }

    HM_TPL
    inline HM_TYPE::~HashMap() {
        for (uint32_t i=0; i<items_count_; ++i) {
            delete getItemPtr_(i);
        }
    }

    HM_TPL
    inline uint32_t HM_TYPE::findItem(const KeyType& key) {
        return findInternal_(key, hashKey_(key));
    }

    HM_TPL
    inline bool HM_TYPE::removeItem(const KeyType& key) {
        uint32_t hash = hashKey_(key);
        uint32_t id = findInternal_(key, hash);
        if (id == InvalidId())
            return false;

        ASSERT(cmp_(key, (const KeyType&)keys_[id]));

        removeLink_(id, hash);
        uint32_t last_item_id = items_count_-1;
        if (last_item_id != id) {
            ItemType* last_item = getItemPtr_(last_item_id);
            // remove last pair from its list
            uint32_t last_hash = hashKey_(keys_[last_item_id]);
            removeLink_(last_item_id, last_hash);

            // move to freed slot and add its link to list
            memcpy(getItemPtr_(id), getItemPtr_(last_item_id), sizeof(ItemType));
            keys_[id] = keys_[last_item_id];
            next_[id] = hash_table_[last_hash];
            hash_table_[last_hash] = id;
        }

        --items_count_;
        return true;
    }

    HM_TPL
    inline const KeyType& HM_TYPE::getKey(uint32_t item_id) const {
        return keys_[item_id];
    }

    HM_TPL
    inline ItemType& HM_TYPE::getItem(uint32_t item_id) {
        return *getItemPtr_(item_id);
    }

    HM_TPL
    inline const ItemType& HM_TYPE::getItem(uint32_t item_id)const {
        return *getItemPtr_(item_id);
    }

    HM_TPL
    inline uint32_t HM_TYPE::getItemIndex(const ItemType* i) const {
        return (size_t(i) - size_t(&data_[0]))/sizeof(ItemType);
    }

    HM_TPL
    inline bool HM_TYPE::isPowerOfTwo_(uint32_t x) {
        while (((x % 2) == 0) && x > 1)
            x /= 2;
        return (x == 1);
    }

    HM_TPL
    inline uint32_t HM_TYPE::findInternal_(const KeyType& key, uint32_t hash) {
        uint32_t id = hash_table_[hash];
        while (id != InvalidId()) {
            const KeyType& k2 = keys_[id];
            if (cmp_(key, k2)) {
                return id;
            }
            id = next_[id];
        }
        return InvalidId();
    }

    HM_TPL
    inline void HM_TYPE::removeLink_(uint32_t id, uint32_t hash) {
    // remove from linked list and stitch
        uint32_t prev_offset = InvalidId();
        uint32_t offset = hash_table_[hash];

        while (offset != id) {
            prev_offset = offset;
            offset = next_[offset];
        }

        if (prev_offset == InvalidId()) {
            hash_table_[hash] = next_[id];
        }
        else {
            ASSERT(next_[prev_offset] == id);
            next_[prev_offset] = next_[id];
        }

#ifdef DEBUG_BUILD
        next_[id]=InvalidId();
#endif
    }

    HM_TPL
    inline ItemType* HM_TYPE::getItemPtr_(uint32_t id)const {
        return (ItemType*)&data_[id*sizeof(ItemType)];
    }

    HM_TPL
    inline uint32_t HM_TYPE::hashKey_(const KeyType& key) {
        return hash_(key) & modulo_mask_;
    }

    HM_TPL
    inline ItemType* HM_TYPE::addItemInner_(uint32_t inner_hash, uint32_t hash, const KeyType& key) {
        if (items_count_ >= hash_table_.size()) {
            uint32_t new_size = calcNextPowerOfTwo(items_count_+1);
            modulo_mask_ = new_size - 1;
            hash_table_.resize(new_size);
            std::fill(hash_table_.begin(), hash_table_.end(), InvalidId());
            data_.resize(new_size*sizeof(ItemType));
            keys_.resize(new_size);
#ifdef DEBUG_BUILD
            next_.resize(new_size, InvalidId());
#else
            next_.resize(new_size);
#endif
            // recalc hashes for keys
            for (uint32_t i=0; i<items_count_; ++i) {
                uint32_t new_hash = hashKey_(keys_[i]);
                next_[i] = hash_table_[new_hash];
                hash_table_[new_hash] = i;
            }

            hash = inner_hash & modulo_mask_; // recompute hash for added item with new modulo
        }

        uint32_t id = items_count_;
        keys_[id] = key;
        next_[id] = hash_table_[hash];
        hash_table_[hash] = id;
        ++items_count_;
        return getItemPtr_(id);
    }
}

#undef HM_TPL
#undef HM_TYPE