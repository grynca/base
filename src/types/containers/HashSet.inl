#include "HashSet.h"
#include "../Index.h"
#include "../../functions/common.h"

#define HS_TPL template <typename KeyType, typename Hasher, typename Compare>
#define HS_TYPE HashSet<KeyType, Hasher, Compare>
#define DEF_SIZE 64

namespace grynca {

    HS_TPL
    inline HS_TYPE::HashSet()
     : HashSet(DEF_SIZE)
    {}

    HS_TPL
    inline HS_TYPE::HashSet(uint32_t initial_size)
     : items_count_(0), modulo_mask_(initial_size -1), initial_size_(initial_size)
    {
        ASSERT(isPowerOfTwo_(initial_size));
        hash_table_.resize(initial_size, InvalidId());
        keys_.resize(initial_size);
#ifdef DEBUG_BUILD
        next_.resize(initial_size, InvalidId());
#else
        next_.resize(initial_size);
#endif
    }

    HS_TPL
    inline void HS_TYPE::reserve(uint32_t size) {
        hash_table_.reserve(size);
        keys_.reserve(size);
        next_.reserve(size);
    }

    HS_TPL
    inline void HS_TYPE::clear() {
        std::fill(hash_table_.begin(), hash_table_.end(), InvalidId());
        std::fill(next_.begin(), next_.end(), InvalidId());
        items_count_ = 0;
    }

    HS_TPL
    inline uint32_t HS_TYPE::addItem(const KeyType& key) {
        uint32_t inner_hash = hash_(key);
        uint32_t hash = inner_hash & modulo_mask_;
        uint32_t id = findInternal_(key, hash);
        if (id != InvalidId())
            // already there
            return id;

        return addItemInner_(inner_hash, hash, key);
    }

    HS_TPL
    inline uint32_t HS_TYPE::addItem(const KeyType& key, bool& was_added) {
        uint32_t inner_hash = hash_(key);
        uint32_t hash = inner_hash & modulo_mask_;
        uint32_t id = findInternal_(key, hash);
        if (id != InvalidId()) {
            // already there
            was_added = false;
            return id;
        }

        was_added = true;
        return addItemInner_(inner_hash, hash, key);
    }

    HS_TPL
    inline uint32_t HS_TYPE::findItem(const KeyType& key) {
        return findInternal_(key, hashKey_(key));
    }

    HS_TPL
    inline bool HS_TYPE::removeItem(const KeyType& key) {
        uint32_t hash = hashKey_(key);
        uint32_t id = findInternal_(key, hash);
        if (id == InvalidId())
            return false;

        ASSERT(cmp_(key, (const KeyType&)keys_[id]));

        removeLink_(id, hash);
        uint32_t last_item_id = items_count_-1;
        if (last_item_id != id) {
            // remove last pair from its list
            uint32_t last_hash = hashKey_(keys_[last_item_id]);
            removeLink_(last_item_id, last_hash);

            // move to freed slot and add its link to list
            keys_[id] = keys_[last_item_id];
            next_[id] = hash_table_[last_hash];
            hash_table_[last_hash] = id;
        }

        --items_count_;
        return true;
    }

    HS_TPL
    inline bool HS_TYPE::isPowerOfTwo_(uint32_t x) {
        while (((x % 2) == 0) && x > 1)
            x /= 2;
        return (x == 1);
    }

    HS_TPL
    inline uint32_t HS_TYPE::findInternal_(const KeyType& key, uint32_t hash) {
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

    HS_TPL
    inline void HS_TYPE::removeLink_(uint32_t id, uint32_t hash) {
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

    HS_TPL
    inline uint32_t HS_TYPE::hashKey_(const KeyType& key) {
        return hash_(key) & modulo_mask_;
    }

    HS_TPL
    inline uint32_t HS_TYPE::addItemInner_(uint32_t inner_hash, uint32_t hash, const KeyType& key) {
        if (items_count_ >= hash_table_.size()) {
            uint32_t new_size = calcNextPowerOfTwo(items_count_+1);
            modulo_mask_ = new_size - 1;
            hash_table_.resize(new_size);
            std::fill(hash_table_.begin(), hash_table_.end(), InvalidId());
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
        return id;
    }
}

#undef HS_TPL
#undef HS_TYPE