#ifndef TYPE_H
#define TYPE_H

#include "functions/defs.h"
#include  "containers/fast_vector.h"
#include "../functions/meta.h"
#include "3rdp/ustring.h"

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
        static ustring getDebugString(ustring indent);
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
        template <typename... ConstructionArgs>
        static void create2(void* place, ConstructionArgs&&... args);
        static void defConstruct(void* place);
        static void destroy(void* place);
        static void copy(void *to, const void *from);
        static void move(void *to, void *from);
        template <typename BaseT>
        static BaseT* castToBase(void* place);

        // these ids are automatically set (compilation order dependant)
        //  should not be communicated over network or saved between runs
        static u32 getInternalTypeId();
        static const TypeInfo& getInternalTypeInfo();
        static std::string getTypename();
        static size_t getSize();

        static const TypeInfo& getTypeInfo();
        static bool isTypeInfoSet();
    private:
        template <typename TT> friend class TypeInfoManager;
        static u32& typeId_();
    };

    // Dummy type, with no-op functions
    struct DummyType {};

    template <typename Domain>
    class Type<DummyType, Domain> {
    public:
        template <typename... ConstructionArgs>
        static void create2(void*, ConstructionArgs&&...) {}
        static void defConstruct(void*) {}
        static void destroy(void*) {}
        static void copy(void*, const void*) {}
        static void move(void*, void*) {}
        template <typename BaseT>
        static BaseT* castToBase(void*) { return NULL; }
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
        const ustring& getTypename()const;

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
        ustring typename_;
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
}

#include "Type.inl"
#endif // TYPE_H
