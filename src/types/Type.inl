#include "Type.h"
#include "Type_internal.h"

/// Inline implementation

namespace grynca {

    template<typename Domain>
    inline uint32_t TypeDomain<Domain>::getNewId() {
        // static
        static uint32_t id_pool = 0;
        return id_pool++;
    }

    template <typename T, typename Domain>
    inline void Type<T, Domain>::destroy(void* place) {
        //static
        (void)place; // so compiler does not complain
        ((T*)place)->~T();
    }

    template <typename T, typename Domain>
    inline uint32_t Type<T, Domain>::getInternalTypeId() {
        //static
        static uint32_t id = TypeDomain<Domain>::getNewId();
        return id;
    }

    template <typename T, typename Domain>
     std::string Type<T, Domain>::getTypename() {
        //static
        return typeid(T()).name();
    };

    template <typename T, typename Domain>
    inline size_t Type<T, Domain>::getSize() {
        //static
        return sizeof(T);
    }

    template <typename T, typename Domain>
    inline const TypeInfo& Type<T, Domain>::getTypeInfo() {
    //static
        return TypeInfoManager<Domain>::get(typeId_());
    }

    template <typename T, typename Domain>
    inline uint32_t& Type<T, Domain>::typeId_() {
    //static
        static uint32_t tid_= uint32_t(-1);
        return tid_;
    }

    template <typename T, typename Domain>
    inline void Type<T, Domain>::copy(void* to, const void *from) {
        //static
        new (to) T( *(const T*)from);
    }

    template <typename T, typename Domain>
    inline void Type<T, Domain>::move(void *to, void *from)  {
        std::move((T*)from, ((T*)from)+1, (T*)to);
    }


    inline TypeInfo::TypeInfo()
     : destroy_(NULL), copy_(NULL), move_(NULL), size_(0), id_(uint32_t(-1))
    {}

    inline bool TypeInfo::isNull()const {
        return destroy_==NULL;
    }

    inline size_t TypeInfo::getSize()const {
        return size_;
    }

    inline DestroyFunc TypeInfo::getDestroyFunc()const {
        return destroy_;
    }

    inline CopyFunc TypeInfo::getCopyFunc()const {
        return copy_;
    }

    inline MoveFunc TypeInfo::getMoveFunc()const {
        return move_;
    }

    inline const std::string& TypeInfo::getTypename()const {
        return typename_;
    }

    template <typename T, typename Domain>
    inline void TypeInfo::set(uint32_t id) {
        id_ = id;
        size_ = Type<T, Domain>::getSize();
        typename_ = Type<T, Domain>::getTypename();
        destroy_ = &Type<T, Domain>::destroy;
        copy_ = &Type<T, Domain>::copy;
        move_ = &Type<T, Domain>::move;
    }

    inline uint32_t TypeInfo::getId()const {
        return id_;
    }

    template <typename Domain>
    template <typename T>
    inline void TypeInfoManager<Domain>::setTypeId(uint32_t tid) {
    //static
        ASSERT(!isTypeIdSet(tid),
               "Type with this id already set.");
        if (tid >=getTypes_().size())
            getTypes_().resize(tid+1);
        getTypes_()[tid].template set<T, Domain>(tid);
        Type<T, Domain>::typeId_() = tid;
    }

    template <typename Domain>
    inline const TypeInfo& TypeInfoManager<Domain>::get(uint32_t tid) {
    //static
        ASSERT(isTypeIdSet(tid),
               "Type with this id not set.");
        return getTypes_()[tid];
    }

    template <typename Domain>
    inline bool TypeInfoManager<Domain>::isTypeIdSet(uint32_t tid) {
    //static
        if (tid >= getTypes_().size())
            return false;
        return !getTypes_()[tid].isNull();
    }

    template <typename Domain>
    inline uint32_t TypeInfoManager<Domain>::getTypesCount() {
    //static
        return uint32_t(getTypes_().size());
    }

    template <typename Domain>
    inline fast_vector<TypeInfo>& TypeInfoManager<Domain>::getTypes_() {
    //static
        static fast_vector<TypeInfo> types_;
        return types_;
    }

    template <typename F, typename... R>
    inline constexpr int TypesPack<F, R...>::getTypesCount() {
    //static
        return sizeof...(R) +1;
    }

    template <typename F, typename... R>
    template <typename T>
    inline constexpr int TypesPack<F, R...>::pos() {
    //static
        return position<T, F, R...>::pos;
    }

    template <typename F, typename... R>
    inline constexpr int TypesPack<F, R...>::empty() {
    //static
        return false;
    }

    template <typename F, typename... R>
    inline const TypeInfo& TypesPack<F, R...>::getTypeInfo(int pos) {
    //static
        static internal::TypeIdTypesPackSetter<Types> lazy_setter_;

        ASSERT(pos < getTypesCount(), "Invalid type pos.");
        return TypeInfoManager<Types>::get(pos);
    }

    template <typename F, typename... R>
    template <typename... Tss>
    inline TypesPack<F, R...,Tss...> TypesPack<F, R...>::expand() {
    //static
        return TypesPack<F, R...,Tss...>();
    }

}
