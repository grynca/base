#ifndef HASHMAP_H
#define HASHMAP_H

#include "fast_vector.h"

namespace grynca {

    // e.g:
    /* struct MyHasher {
     *    uint32_t operator()(const MyKey& k) {
     *       return some_magic_hashing(...);
     *    }
     * }
     *
     * struct MyCmp {
     *    bool operator()(const MyKey& k1, const MyKey& k2) {
     *       return k1.bla1 == k2.bla1 && k1.bla2 == k2.bla2;
     *    }
     * }
     */

    template <typename ItemType, typename KeyType, typename Hasher = std::hash<KeyType>, typename Compare = std::equal_to<KeyType> >
    class HashMap {
    public:
        // size/initial_size must be power of 2
        HashMap();
        HashMap(uint32_t initial_size);
        ~HashMap();

        void reserve(uint32_t size);
        void clear();
        ItemType* addItem(const KeyType& key);   // returns item ptr where user must construct item himself
        ItemType* addItem(const KeyType& key, bool& was_added);
        uint32_t findItem(const KeyType& key);      // returns Invalid() if not found
        bool removeItem(const KeyType& key);        // return true if item was found and removed

        uint32_t getItemsCount() const { return items_count_; }
        const KeyType& getKey(uint32_t item_id) const;
        ItemType& getItem(uint32_t item_id);
        const ItemType& getItem(uint32_t item_id)const;
        uint32_t getItemIndex(const ItemType* i) const;     // ptr must point inside map

        Hasher& accHasher() { return hash_; }
        Compare acComparer() { return cmp_; }
    private:
        bool isPowerOfTwo_(uint32_t x);
        uint32_t findInternal_(const KeyType& key, uint32_t hash);      // returns index
        void removeLink_(uint32_t id, uint32_t hash);
        ItemType* getItemPtr_(uint32_t id)const;
        uint32_t hashKey_(const KeyType& key);
        ItemType* addItemInner_(uint32_t inner_hash, uint32_t hash, const KeyType& key);

        fast_vector<uint32_t> hash_table_;
        fast_vector<uint8_t> data_;
        fast_vector<KeyType> keys_;
        fast_vector<uint32_t> next_;
        uint32_t items_count_;
        uint32_t modulo_mask_;
        uint32_t initial_size_;
        Hasher hash_;
        Compare cmp_;
    };

}

#include "HashMap.inl"
#endif //HASHMAP_H
