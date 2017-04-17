#include "Variant.h"
#include "VariantCaller.h"
#include <cassert>

namespace grynca
{
    template <typename ... Ts>
    inline Variant<Ts...>::Variant()
        : curr_pos_(-1)
    {
    }

    template <typename ... Ts>
    inline Variant<Ts...>::Variant(const Variant<Ts...>& old)
        : curr_pos_(old.curr_pos_)
    {
        if (curr_pos_ != -1)
            getHelper_().copy(curr_pos_, &data_, &old.data_);
    }

    template <typename ... Ts>
    inline Variant<Ts...>::Variant(Variant<Ts...>&& old)
        : curr_pos_(old.curr_pos_)
    {
        if (curr_pos_ != -1)
            getHelper_().move(curr_pos_, &data_, &old.data_);
    }

    template <typename ... Ts>
    template <typename T>
    inline Variant<Ts...>::Variant(const T& t) {
        // construct new with placement new
        new (&data_) T(t);
        curr_pos_ = getTypeIdOf<T>();
    }

    template <typename ... Ts>
    template <typename T>
    inline Variant<Ts...>::Variant(T&& t) {
        // construct new with placement new
        new (&data_) T(t);
        curr_pos_ = getTypeIdOf<T>();
    }

    template <typename ... Ts>
    inline Variant<Ts...>::~Variant() {
        if (curr_pos_ != -1)
            getHelper_().destroy(curr_pos_, &data_);
    }

    template <typename ... Ts>
    inline Variant<Ts...>& Variant<Ts...>::operator= (const Variant<Ts...>& v) {
        curr_pos_ = v.curr_pos_;
        if (curr_pos_ != -1)
            getHelper_().copy(curr_pos_, &data_, &v.data_);
        return *this;
    }

    template <typename ... Ts>
    inline Variant<Ts...>& Variant<Ts...>::operator= (Variant<Ts...>&& v) {
        curr_pos_ = v.curr_pos_;
        if (curr_pos_ != -1)
            getHelper_().move(curr_pos_, &data_, &v.data_);
        return *this;
    }

    template <typename ... Ts>
    template <typename T>
    inline Variant<Ts...>& Variant<Ts...>::operator= (T& t) {
        set(t);
        return *this;
    }


    template <typename ... Ts>
    template<typename T>
    inline bool Variant<Ts...>::is()const {
        return (curr_pos_ == getTypeIdOf<T>() );
    }

    template <typename ... Ts>
    inline bool Variant<Ts...>::valid()const {
        return (curr_pos_ != -1);
    }

    template <typename ... Ts>
    template<typename T, typename... Args>
    inline T& Variant<Ts...>::set(Args&&... args) {
        // First we destroy the current contents
        if (curr_pos_ != -1)
            getHelper_().destroy(curr_pos_, &data_);
        // construct new with placement new
        new (&data_) T(std::forward<Args>(args)...);
        curr_pos_ = getTypeIdOf<T>();
        return get<T>();
    }

    template <typename ... Ts>
    inline void Variant<Ts...>::unset() {
        if (curr_pos_ != -1)
            getHelper_().destroy(curr_pos_, &data_);
    }

    template <typename ... Ts>
    template <typename T>
    inline T& Variant<Ts...>::get() {
        ASSERT_M(getTypeIdOf<T>() == curr_pos_,
               "This type is not currently set in Variant.");
        return *(T*)(&data_);
    }

    template <typename ... Ts>
    template<typename IfaceT>
    inline IfaceT& Variant<Ts...>::getBase() {
        IfaceT *ptr = NULL;
        VariantCaller<internal::VariantIfaceCaster>::call<Variant<Ts...> >(*this, ptr);
        return *ptr;
    }

    template <typename ... Ts>
    template<typename IfaceT>
    inline const IfaceT& Variant<Ts...>::getBase()const {
        IfaceT *ptr = NULL;
        VariantCaller<internal::VariantIfaceCaster>::call<Variant<Ts...> >(*const_cast<Variant<Ts...>*>(this), ptr);
        return *ptr;
    }

    template <typename ... Ts>
    template<typename T>
    inline const T& Variant<Ts...>::get()const {
        ASSERT_M(getTypeIdOf<T>() == curr_pos_,
               "This type is not currently set in Variant.");
        return *(const T*)(&data_);
    }

    template <typename ... Ts>
    inline void* Variant<Ts...>::getData() {
        return &data_;
    }

    template <typename ... Ts>
    inline const void* Variant<Ts...>::getData()const {
        return &data_;
    }

    template <typename ... Ts>
    template<typename T>
    inline constexpr int Variant<Ts...>::getTypeIdOf() {
    // static
        return position<T, Ts...>::pos;
    }

    template <typename ... Ts>
    inline internal::VariantHelper<Ts...>& Variant<Ts...>::getHelper_() {
        static internal::VariantHelper<Ts...> helper;
        return helper;
    }
}