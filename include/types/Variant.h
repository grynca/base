#ifndef VARIANT_H
#define VARIANT_H

#include "Type.h"
#include "variant_helper.h"
#include "../functions/meta.h"

#define GET_BASE(e, BASE) ((BASE&)(e).template getBase<BASE>())

namespace grynca {

    template <typename ... Ts>
    class Variant {
    public:
        typedef TypesPack<Ts...> Types;

        Variant();
        Variant(const Variant<Ts...>& old);
        Variant(Variant<Ts...>&& old);
        template <typename T>
        Variant(const T& t);
        template <typename T>
        Variant(T&& t);

        ~Variant();

        Variant<Ts...>& operator= (const Variant<Ts...>& v);
        Variant<Ts...>& operator= (Variant<Ts...>&& v);

        template <typename T>
        Variant<Ts...>& operator= (T& t);

        template <typename T>
        bool is()const;

        bool valid()const;

        template<typename T, typename... Args>
        T& set(Args&&... args);

        void unset();

        template<typename T>
        T& get();

        template<typename IfaceT>
        IfaceT& getBase();

        template<typename IfaceT>
        const IfaceT& getBase()const;

        template<typename T>
        const T& get()const;

        void* getData();
        const void* getData()const ;

        int getTypeId()const { return curr_pos_; }

        template <typename T>
        static constexpr int getTypeIdOf();
    protected:

        internal::VariantHelper<Ts...>& getHelper_();

        static const size_t data_size = static_max<sizeof(Ts)...>::value;
        static const size_t data_align = static_max<alignof(Ts)...>::value;

        using data_t = typename std::aligned_storage<data_size, data_align>::type;

        data_t data_;
        i32 curr_pos_;
    };

    // Variant from TypesPack
    template <typename... Ts>
    class Variant<TypesPack<Ts...> > : public Variant<Ts...> {};
}

#include "Variant.inl"

#endif //VARIANT_H
