#ifndef VARIANT_H
#define VARIANT_H

#include "Type.h"
#include "variant_helper.h"
#include "../functions/meta.h"
#include <ostream>

namespace grynca {

    template <typename ... Ts>
    class Variant {
    public:
        Variant();
        Variant(const Variant<Ts...>& old);
        Variant(Variant<Ts...>&& old);

        ~Variant();

        // Serves as both the move and the copy asignment operator.
        Variant<Ts...>& operator= (Variant<Ts...> old);

        template <typename T>
        bool is();

        bool valid();

        template<typename T, typename... Args>
        T& set(Args&&... args);

        template<typename T>
        T& get();

        template<typename IfaceT>
        IfaceT& getBase();

        template<typename T>
        const T& get()const;

        void* getData();

        typedef TypesPack<Ts...> Types;
        int getCurrentType()const { return curr_pos_; }

        template <typename T>
        static int typePos();
    private:

        internal::VariantHelper<Ts...>& getHelper_();

        static const size_t data_size = static_max<sizeof(Ts)...>::value;
        static const size_t data_align = static_max<alignof(Ts)...>::value;

        using data_t = typename std::aligned_storage<data_size, data_align>::type;

        data_t data_;
        int curr_pos_;
    };

    template <typename... Ts>
    class Variant<TypesPack<Ts...> > : public Variant<Ts...> {};


}

#include "Variant.inl"

#endif //VARIANT_H
