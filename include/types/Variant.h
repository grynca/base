#ifndef VARIANT_H
#define VARIANT_H

#include "Type.h"
#include "variant_helper.h"
#include "../functions/meta.h"

#define GET_BASE(e, BASE) ((BASE&)(e).template getBase<BASE>())

namespace grynca {

    template <typename ... Ts>
    class Variant {
        typedef TypesPack<DummyType, Ts...> TypesWithDummy;
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
        bool is()const;

        bool valid()const;

        template<typename T, typename... Args>
        T& create(Args&&... args);  // first "set" does not care for unsetting previous

        template<typename T>
        T& set(const T& src);
        template<typename T>
        T& set(T&& src);
        template<typename T, typename... Args>
        T& set(Args&&... args);
        template<typename... Args>
        void setByTypeId(i32 type_id, Args&&... args);      // all types must have this constructor in order for this to compile
        void setFromData(i32 type_id, const void* source);  // source must contain correct object that will be copied to Variant
        void unset();

        template<typename T>
        const T& get()const;
        template<typename T>
        T& acc();

        template<typename BaseT>
        BaseT& accBase();
        template<typename BaseT>
        const BaseT& getBase()const;

        void* accData();
        const void* getData()const;
        static u32 getDataSize();

        i32 getTypeId()const { return type_id_; }

        template <typename T>
        static constexpr int getTypeIdOf();

        template <typename Functor, typename... Args>
        constexpr auto callFunctor(Args&&...)const;

        // for those items that are of Type that fullfils condition Cond call TrueFunctor,
        // for others call FalseFunctor
        template <typename Cond, typename TrueFunctor, typename FalseFunctor = EmptyFunctorT, typename... Args>
        constexpr auto callFunctorCond(Args&&...);
    protected:
        template<typename T, typename... Args>
        T& innerSet_(Args&&... args);

        static const size_t data_size = static_max<sizeof(Ts)...>::value;
        static const size_t data_align = static_max<alignof(Ts)...>::value;

        using data_t = typename std::aligned_storage<data_size, data_align>::type;

        data_t data_;
        i32 type_id_;
    };

    // Variant from TypesPack
    template <typename... Ts>
    class Variant<TypesPack<Ts...> > : public Variant<Ts...> {};
}

#include "Variant.inl"

#endif //VARIANT_H
