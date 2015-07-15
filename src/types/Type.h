#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <algorithm>
#include <cassert>
#include "../functions/meta.h"

namespace grynca {
    typedef void (*DestroyFunc)(void*);
    typedef void (*CopyFunc)(void*, const void*);
    typedef void (*MoveFunc)(void*, void*);

    template<typename Domain>
    class TypeIdGiver {
    protected:
        static uint32_t getNewId();
    };

    template <typename T, typename Domain = void>
    class Type : public TypeIdGiver<Domain>
    {
    public:
        static void destroy(void* place);
        static void copy(void *to, const void *from);
        static void move(void *to, void *from);

        // these ids are automatically set (compilation order dependant)
        //  should not be communicated over network or saved between runs
        static uint32_t getInternalTypeId();
    };

    // class with settable class id
    template <typename T>
    class CustomTypeId
    {
    public:
        static uint32_t getTypeId();
        static void setTypeId(uint32_t tid);
        static bool isTypeIdSet();

    private:
        static int32_t& tid_();
    };


    #define IF_EMPTY(TP) typename std::enable_if< TP::empty() >::type
    #define IF_NOT_EMPTY(TP) typename std::enable_if< !TP::empty() >::type
    #define HEAD(TP) typename TP::Head
    #define TAIL(TP) typename TP::Tail

    template <typename...Ts>  class TypesPack;

    template <>
    class TypesPack<> {
    public:
        static constexpr int getTypesCount() { return 0;}

        // get type position id in pack
        template <typename T>
        static constexpr int pos() { return position<T>::pos; }

        static constexpr int empty() { return true; }
    };

    template <typename F, typename... R>
    class TypesPack<F, R...> {
    public:
        typedef TypesPack<F,R...> Types;
        typedef F Head;
        typedef TypesPack<R...> Tail;


        static constexpr int getTypesCount() {
            return sizeof...(R) +1;
        }

        // get type position id in pack
        template <typename T>
        static constexpr int pos() {
            return position<T, F, R...>::pos;
        }

        static constexpr int empty() {
            return false;
        }
    };
}

#include "Type.inl"
#endif // TYPE_H
