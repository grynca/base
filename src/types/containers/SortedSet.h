#ifndef SIMPLESET_H
#define SIMPLESET_H
#include "fast_vector.h"

namespace grynca {

    /*
     *  keeps items sorted
     *  items stored in tight linear array
     *  fast looping
     *  slower add/remove (memmove because of sorting)
     */

    template <typename ItemType>
    class SortedSet {
    public:
        template <typename ... ConstrArgs>
        ItemType& add(uint32_t id, ConstrArgs&& ... args);

        ItemType& get(uint32_t id);
        bool isContained(uint32_t id)const;
        const ItemType& get(uint32_t id)const;
        ItemType& getAtPos(uint32_t pos);
        const ItemType& getAtPos(uint32_t pos)const;

        void remove(uint32_t id);
        void removeAtPos(uint32_t pos);

        uint32_t getIndexForPos(uint32_t pos)const;
        uint32_t getPosForIndex(uint32_t id)const;

        uint32_t size()const;
        void reserve(uint32_t size);
        bool empty();
        void clear();

    private:
        fast_vector<uint32_t> positions_;
        fast_vector<ItemType> items_;
        fast_vector<uint32_t> ids_;
    };

}


#include "SortedSet.inl"
#endif //SIMPLESET_H
