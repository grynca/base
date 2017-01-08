#ifndef HASHSET_H
#define HASHSET_H

#include "fast_vector.h"

namespace grynca {

    template <typename KeyType, typename Hasher = std::hash<KeyType>, typename Compare = std::equal_to<KeyType> >
    class HashSet {
    public:
        // size/initial_size must be power of 2
        HashSet();
        HashSet(u32 initial_size);

        void reserve(u32 size);
        void clear();
        u32 addItem(const KeyType& key);       // returns added item index
        u32 addItem(const KeyType& key, bool& was_added);
        u32 findItem(const KeyType& key);      // InvalidId() if not found
        bool removeItem(const KeyType& key);        // return true if item was found and removed

        u32 getItemsCount() const { return items_count_; }
        const KeyType& getItem(u32 item_id) const { return keys_[item_id]; }
        Hasher& accHasher() { return hash_; }
        Compare acComparer() { return cmp_; }
    private:
        bool isPowerOfTwo_(u32 x);
        u32 findInternal_(const KeyType& key, u32 hash);      // returns index
        void removeLink_(u32 id, u32 hash);
        u32 hashKey_(const KeyType& key);
        u32 addItemInner_(u32 inner_hash, u32 hash, const KeyType& key);

        fast_vector<u32> hash_table_;
        fast_vector<KeyType> keys_;
        fast_vector<u32> next_;
        u32 items_count_;
        u32 modulo_mask_;
        u32 initial_size_;
        Hasher hash_;
        Compare cmp_;
    };

}


#include "HashSet.inl"
#endif //HASHSET_H
