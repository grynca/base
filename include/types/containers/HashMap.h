#ifndef HASHMAP_H
#define HASHMAP_H

#include "fast_vector.h"

namespace grynca {

    // e.g:
    /* struct MyHasher {
     *    u32 operator()(const MyKey& k) {
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

    template <typename ItemType, typename KeyType, typename Hasher, typename Compare = std::equal_to<KeyType> >
    class HashMap {
    public:
        // size/initial_size must be power of 2
        HashMap();
        HashMap(u32 initial_size);
        ~HashMap();

        void reserve(u32 size);
        void clear();
        ItemType* addItem(const KeyType& key);   // returns item ptr where user must construct item himself
        ItemType* findOrAddItem(const KeyType &key, bool &was_added);
        u32 findItemId(const KeyType& key)const;      // returns InvalidId() if not found
        const ItemType* findItem(const KeyType& key)const;
        ItemType* findItem(const KeyType& key);
        bool removeItem(const KeyType& key);        // return true if item was found and removed

        u32 getItemsCount() const { return items_count_; }
        const KeyType& getKey(u32 item_id) const;
        ItemType& getItem(u32 item_id);
        const ItemType& getItem(u32 item_id)const;
        u32 getItemIndex(const ItemType* i) const;     // ptr must point inside map

        Hasher& accHasher() { return hash_; }
        Compare acComparer() { return cmp_; }
    private:
        bool isPowerOfTwo_(u32 x);
        u32 findInternal_(const KeyType& key, u32 hash)const;      // returns index
        ItemType* findInternal2_(const KeyType& key, u32 hash)const;
        void removeLink_(u32 id, u32 hash);
        ItemType* getItemPtr_(u32 id)const;
        u32 hashKey_(const KeyType& key)const;
        ItemType* addItemInner_(u32 inner_hash, u32 hash, const KeyType& key);

        fast_vector<u32> hash_table_;
        fast_vector<u8> data_;
        fast_vector<KeyType> keys_;
        fast_vector<u32> next_;
        u32 items_count_;
        u32 modulo_mask_;
        u32 initial_size_;
        Hasher hash_;
        Compare cmp_;
    };

}

#include "HashMap.inl"
#endif //HASHMAP_H
