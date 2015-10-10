#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <algorithm>
#include <cassert>
#include  "containers/fast_vector.h"
#include "../functions/meta.h"

namespace grynca {
    typedef void (*DestroyFunc)(void*);
    typedef void (*CopyFunc)(void* to, const void* from);
    typedef void (*MoveFunc)(void* to, void* from);

    // fw
    class TypeInfo;
    template<typename> class TypeInfoManager;

    template<typename Domain>
    class TypeDomain {
    protected:
        static uint32_t getNewId();
    };

// Static type info
    template <typename T, typename Domain = void>
    class Type : public TypeDomain<Domain>
    {
    public:
        static void destroy(void* place);
        static void copy(void *to, const void *from);
        static void move(void *to, void *from);

        // these ids are automatically set (compilation order dependant)
        //  should not be communicated over network or saved between runs
        static uint32_t getInternalTypeId();
        static std::string getTypename();
        static size_t getSize();

        static const TypeInfo& getTypeInfo();
    private:
        template <typename TT> friend class TypeInfoManager;
        static uint32_t& typeId_();
    };

// Dynamic type info
    class TypeInfo {
    public:
        TypeInfo();

        bool isNull()const;
        size_t getSize()const;
        DestroyFunc getDestroyFunc()const;
        CopyFunc getCopyFunc()const;
        MoveFunc getMoveFunc()const;
        const std::string& getTypename()const;

        template <typename T, typename Domain>
        void set(uint32_t id);

        uint32_t getId()const;
    private:
        DestroyFunc destroy_;
        CopyFunc copy_;
        MoveFunc move_;
        size_t size_;
        uint32_t id_;
        std::string typename_;
    };

    template<typename Domain = void>
    class TypeInfoManager
    {
    public:
        template <typename T>
        static void setTypeId(uint32_t tid);
        static const TypeInfo& get(uint32_t tid);
        static bool isTypeIdSet(uint32_t tid);
        static uint32_t getTypesCount();
    protected:
        static fast_vector<TypeInfo>& getTypes_();
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

        template <typename... Tss>
        static TypesPack<Tss...> expand() {
            return TypesPack<Tss...>();
        }
    };

    template <typename F, typename... R>
    class TypesPack<F, R...> {
    public:
        typedef TypesPack<F,R...> Types;
        typedef F Head;
        typedef TypesPack<R...> Tail;

        static constexpr int getTypesCount();

        template <typename T>
        static constexpr int pos(); // get type position id in pack

        static constexpr int empty();

        static const TypeInfo& getTypeInfo(int pos);

        template <typename... Tss>
        static TypesPack<F, R...,Tss...> expand();  // expand with types

    };


    template <typename... Ts> struct TypesPackMerge;
    template <typename... Ts1, typename... Ts2>
    struct TypesPackMerge<TypesPack<Ts1...>, TypesPack<Ts2...> > {
        typedef TypesPack<Ts1..., Ts2...> Types;
    };
}

#include "Type.inl"
#endif // TYPE_H
