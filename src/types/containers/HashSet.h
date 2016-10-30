#ifndef HASHSET_H
#define HASHSET_H

#include "fast_vector.h"

namespace grynca {

    template <typename KeyType, typename Hasher = std::hash<KeyType>, typename Compare = std::equal_to<KeyType> >
    class HashSet {
    public:
        // size/initial_size must be power of 2
        HashSet();
        HashSet(uint32_t initial_size);

        void reserve(uint32_t size);
        void clear();
        uint32_t addItem(const KeyType& key);       // returns added item index
        uint32_t addItem(const KeyType& key, bool& was_added);
        uint32_t findItem(const KeyType& key);      // InvalidId() if not found
        bool removeItem(const KeyType& key);        // return true if item was found and removed

        uint32_t getItemsCount() const { return items_count_; }
        const KeyType& getItem(uint32_t item_id) const { return keys_[item_id]; }
        Hasher& accHasher() { return hash_; }
        Compare acComparer() { return cmp_; }
    private:
        bool isPowerOfTwo_(uint32_t x);
        uint32_t findInternal_(const KeyType& key, uint32_t hash);      // returns index
        void removeLink_(uint32_t id, uint32_t hash);
        uint32_t hashKey_(const KeyType& key);
        uint32_t addItemInner_(uint32_t inner_hash, uint32_t hash, const KeyType& key);

        fast_vector<uint32_t> hash_table_;
        fast_vector<KeyType> keys_;
        fast_vector<uint32_t> next_;
        uint32_t items_count_;
        uint32_t modulo_mask_;
        uint32_t initial_size_;
        Hasher hash_;
        Compare cmp_;
    };

}


#include "HashSet.inl"
#endif //HASHSET_H
