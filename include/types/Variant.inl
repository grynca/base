#include "Variant.h"
#include "TypedFuncs.h"
#include <cassert>

namespace grynca
{
    template <typename ... Ts>
    inline Variant<Ts...>::Variant()
        : type_id_(-1)
    {
    }

    template <typename ... Ts>
    inline Variant<Ts...>::Variant(const Variant<Ts...>& old)
        : type_id_(old.type_id_)
    {
        auto &f = TypedFuncs<internal::VariantCopyConstructorF, TypesWithDummy>::funcs[type_id_ + 1];
        f.callFor(&data_, &old.data_);
    }

    template <typename ... Ts>
    inline Variant<Ts...>::Variant(Variant<Ts...>&& old)
        : type_id_(old.type_id_)
    {
        auto &f = TypedFuncs<internal::VariantMoveConstructorF, TypesWithDummy>::funcs[type_id_ + 1];
        f.callFor(&data_, &old.data_);
    }

    template <typename ... Ts>
    template <typename T>
    inline Variant<Ts...>::Variant(const T& t) {
        // construct new with placement new
        new (&data_) T(t);
        type_id_ = getTypeIdOf<T>();
    }

    template <typename ... Ts>
    template <typename T>
    inline Variant<Ts...>::Variant(T&& t) {
        // construct new with placement new
        new (&data_) T(std::move(t));
        type_id_ = getTypeIdOf<T>();
    }

    template <typename ... Ts>
    inline Variant<Ts...>::~Variant() {
        auto& f = TypedFuncs<internal::VariantDestructorF, TypesWithDummy>::funcs[type_id_+1];
        f.callFor(&data_);
    }

    template <typename ... Ts>
    inline Variant<Ts...>& Variant<Ts...>::operator= (const Variant<Ts...>& v) {
        unset();
        type_id_ = v.type_id_;
        auto &f = TypedFuncs<internal::VariantCopyConstructorF, TypesWithDummy>::funcs[type_id_ + 1];
        f.callFor(&data_, &v.data_);
        return *this;
    }

    template <typename ... Ts>
    inline Variant<Ts...>& Variant<Ts...>::operator= (Variant<Ts...>&& v) {
        unset();
        type_id_ = v.type_id_;
        auto &f = TypedFuncs<internal::VariantMoveConstructorF, TypesWithDummy>::funcs[type_id_ + 1];
        f.callFor(&data_, &v.data_);
        return *this;
    }

    template <typename ... Ts>
    template<typename T>
    inline bool Variant<Ts...>::is()const {
        return (type_id_ == getTypeIdOf<T>() );
    }

    template <typename ... Ts>
    inline bool Variant<Ts...>::valid()const {
        return (type_id_ != -1);
    }

    template <typename ... Ts>
    template<typename T, typename... Args>
    inline T& Variant<Ts...>::create(Args&&... args) {
        new (&data_) T(std::forward<Args>(args)...);
        type_id_ = getTypeIdOf<T>();
        return acc<T>();
    };

    template <typename ... Ts>
    template<typename T>
    inline T& Variant<Ts...>::set(const T& src) {
        return innerSet_<T>(src);
    }

    template <typename ... Ts>
    template<typename T>
    inline T& Variant<Ts...>::set(T&& src) {
        return innerSet_<T>(std::forward<T>(src));
    }

    template <typename ... Ts>
    template<typename T, typename... Args>
    inline T& Variant<Ts...>::set(Args&&... args) {
        return innerSet_<T>(std::forward<Args>(args)...);
    }

    template <typename ... Ts>
    template<typename... Args>
    inline void Variant<Ts...>::setByTypeId(i32 type_id, Args&&... args) {
        unset();
        type_id_ = type_id;
        auto& f = TypedFuncs<internal::VariantConstructorF<Args...>, TypesWithDummy>::funcs[type_id_+1];
        f.callFor(&data_, std::forward(args)...);
    }

    template <typename ... Ts>
    inline void Variant<Ts...>::setFromData(i32 type_id, const void* source) {
        unset();
        type_id_ = type_id;
        auto& f = TypedFuncs<internal::VariantCopyConstructorF, TypesWithDummy>::funcs[type_id_+1];
        f.callFor(&data_, source);
    }

    template <typename ... Ts>
    inline void Variant<Ts...>::unset() {
        auto& f = TypedFuncs<internal::VariantDestructorF, TypesWithDummy>::funcs[type_id_+1];
        f.callFor(&data_);
        type_id_ = -1;
    }

    template <typename ... Ts>
    template<typename T>
    inline T& Variant<Ts...>::acc() {
        ASSERT_M(getTypeIdOf<T>() == type_id_,
                 "This type is not currently set in Variant.");
        return *(T*)(&data_);
    }

    template <typename ... Ts>
    template<typename BaseT>
    inline BaseT& Variant<Ts...>::accBase() {
        ASSERT(valid());
        auto& f = TypedFuncs<internal::BaseCasterF<BaseT>, TypesWithDummy>::funcs[type_id_+1];
        return *f.callFor(&data_);
    }

    template <typename ... Ts>
    template<typename BaseT>
    inline const BaseT& Variant<Ts...>::getBase()const {
        ASSERT(valid());
        auto& f = TypedFuncs<internal::BaseCasterF<BaseT>, TypesWithDummy>::funcs[type_id_+1];
        return *f.callFor(const_cast<data_t*>(&data_));
    }

    template <typename ... Ts>
    template<typename T>
    inline const T& Variant<Ts...>::get()const {
        ASSERT_M(getTypeIdOf<T>() == type_id_,
               "This type is not currently set in Variant.");
        return *(const T*)(&data_);
    }

    template <typename ... Ts>
    inline void* Variant<Ts...>::accData() {
        return &data_;
    }

    template <typename ... Ts>
    inline const void* Variant<Ts...>::getData()const {
        return &data_;
    }

    template <typename ... Ts>
    inline u32 Variant<Ts...>::getDataSize() {
        return sizeof(data_t);
    }

    template <typename ... Ts>
    template<typename T>
    inline constexpr int Variant<Ts...>::getTypeIdOf() {
    // static
        return position<T, Ts...>::pos;
    }

    template <typename ... Ts>
    template <typename Functor, typename... Args>
    inline constexpr auto Variant<Ts...>::callFunctor(Args&&... args)const {
        ASSERT(valid());
        auto& f = TypedFuncs<Functor, Types>::funcs[type_id_];
        return f.callFor((void*)&data_, std::forward<Args>(args)...);
    }

    template <typename ... Ts>
    template <typename Cond, typename TrueFunctor, typename FalseFunctor, typename... Args>
    inline constexpr auto Variant<Ts...>::callFunctorCond(Args&&... args) {
        ASSERT(valid());
        auto& f = TypedFuncs<internal::CallCondF<Cond, TrueFunctor, FalseFunctor>, Types>::funcs[type_id_];
        return f.callFor(&data_, std::forward(args)...);
    }

    template <typename ... Ts>
    template<typename T, typename... Args>
    inline T& Variant<Ts...>::innerSet_(Args&&... args) {
        // First we destroy the current contents
        unset();
        // construct new with placement new
        new (&data_) T(std::forward<Args>(args)...);
        type_id_ = getTypeIdOf<T>();
        return acc<T>();
    }
}