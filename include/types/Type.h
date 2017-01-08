#ifndef TYPE_H
#define TYPE_H

#include "../functions/defs.h"
#include <algorithm>
#include <cassert>
#include  "containers/fast_vector.h"
#include "../functions/meta.h"

namespace grynca {
    typedef void (*DestroyFunc)(void*);
    typedef void (*DefConstrFunc) (void*);
    typedef void (*CopyFunc)(void* to, const void* from);
    typedef void (*MoveFunc)(void* to, void* from);

    // fw
    class TypeInfo;
    template<typename> class TypeInfoManager;

// Type manager for internal types
    template<typename Domain = void>
    class InternalTypes {
    public:
        static const TypeInfo& getInfo(u32 tid);
        static bool isTypeIdSet(u32 tid);
        static std::string getDebugString(std::string indent);
    protected:
        template <typename T>
        static u32 getNewId_();

        // internal type infos
        static fast_vector<TypeInfo>& getTypes_();
    };

// Static type info
    template <typename T, typename Domain = void>
    class Type : public InternalTypes<Domain>
    {
    public:
        template <typename BaseType = T, typename... ConstructionArgs>
        static BaseType* create(void* place, ConstructionArgs&&... args);
        static void defConstruct(void* place);
        static void destroy(void* place);
        static void copy(void *to, const void *from);
        static void move(void *to, void *from);

        // these ids are automatically set (compilation order dependant)
        //  should not be communicated over network or saved between runs
        static u32 getInternalTypeId();
        static std::string getTypename();
        static size_t getSize();

        static const TypeInfo& getTypeInfo();
        static bool isTypeInfoSet();
    private:
        template <typename TT> friend class TypeInfoManager;
        static u32& typeId_();
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
        DefConstrFunc getDefaultConstr()const;
        const std::string& getTypename()const;

        template <typename T, typename Domain>
        void set(u32 id);

        u32 getId()const;

        std::string getDebugString()const;
    private:
        DestroyFunc destroy_;
        DefConstrFunc def_constr_;
        CopyFunc copy_;
        MoveFunc move_;
        size_t size_;
        u32 id_;
        std::string typename_;
    };

// Type manager for user-defined types
    template<typename Domain = void>
    class TypeInfoManager
    {
    public:
        template <typename T>
        static void setTypeId(u32 tid);
        static const TypeInfo& get(u32 tid);
        static bool isTypeIdSet(u32 tid);
        static u32 getTypesCount();
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

        static const TypeInfo& getTypeInfo(int pos) { ASSERT_M(false, "empty types pack"); return Type<void>::getTypeInfo(); }

        template <typename... Tss>
        static TypesPack<Tss...> expand() {
            return TypesPack<Tss...>();
        }

        template <typename Functor, typename... Args>
        static void callOnTypes(Args&&... args) {};
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


        /* functor must be:
            struct Functor {
                template <typename TypesPack, typename Type>
                static void f(args) {}
            }
        */
        template <typename Functor, typename... Args>
        static void callOnTypes(Args&&... args);
    private:
        template <typename TPOrig, typename TP, typename Functor, typename... Args>
        static IF_EMPTY(TP) callOnInner_(Args&&... args);

        template <typename TPOrig, typename TP, typename Functor, typename... Args>
        static IF_NOT_EMPTY(TP) callOnInner_(Args&&... args);
    };


    // usage:
    //   typedef TypesPackMerge<TP1, TP2>::Types MyTypesPack;
    template <typename... Ts> struct TypesPackMerge;
    template <typename... Ts1, typename... Ts2>
    struct TypesPackMerge<TypesPack<Ts1...>, TypesPack<Ts2...> > {
        typedef TypesPack<Ts1..., Ts2...> Types;
    };
}

#include "Type.inl"
#endif // TYPE_H
