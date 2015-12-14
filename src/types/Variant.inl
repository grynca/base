#include "Variant.h"
#include "VariantCaller.h"
#include <algorithm>
#include <utility>
#include <typeinfo>
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
    inline Variant<Ts...>::~Variant() {
        if (curr_pos_ != -1)
            getHelper_().destroy(curr_pos_, &data_);
    }

    template <typename ... Ts>
    inline Variant<Ts...>& Variant<Ts...>::operator= (Variant<Ts...> old) {
        std::swap(curr_pos_, old.curr_pos_);
        std::swap(data_, old.data_);
        return *this;
    }

    template <typename ... Ts>
    template<typename T>
    inline bool Variant<Ts...>::is() {
        return (curr_pos_ == typePos<T>() );
    }

    template <typename ... Ts>
    inline bool Variant<Ts...>::valid() {
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
        curr_pos_ =  typePos<T>();
        return get<T>();
    }

    template <typename ... Ts>
    template <typename T>
    inline T& Variant<Ts...>::get() {
        ASSERT(typePos<T>() == curr_pos_,
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
    template<typename T>
    inline const T& Variant<Ts...>::get()const {
        ASSERT(typePos<T>() == curr_pos_,
               "This type is not currently set in Variant.");
        return *(const T*)(&data_);
    }

    template <typename ... Ts>
    inline void* Variant<Ts...>::getData() {
        return &data_;
    }

    template <typename ... Ts>
    template<typename T>
    inline int Variant<Ts...>::typePos() {
    // static
        return position<T, Ts...>::pos;
    }

    template <typename ... Ts>
    inline internal::VariantHelper<Ts...>& Variant<Ts...>::getHelper_() {
        static internal::VariantHelper<Ts...> helper;
        return helper;
    }
}