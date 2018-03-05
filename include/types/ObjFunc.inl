#include "ObjFunc.h"

#define OF_TPL template<typename TReturn, typename ...Args>
#define OF_TYPE ObjFunc<TReturn(Args...)>

namespace grynca {

    OF_TPL
    inline constexpr OF_TYPE::ObjFunc()
     : obj_(NULL), calling_function_(&noOp_)
    {
    }

    OF_TPL
    template<typename Functor, typename TObject>
    inline constexpr OF_TYPE OF_TYPE::create(TObject *obj) {
        // static
        OF_TYPE f;
        f.bind<Functor>(obj);
        return std::move(f);
    }

    OF_TPL
    template<typename Functor, typename T, typename TObject>
    inline constexpr OF_TYPE OF_TYPE::createT(TObject *obj) {
        // static
        OF_TYPE f;
        f.bindT<Functor, T>(obj);
        return std::move(f);
    }

    OF_TPL
    template<typename Functor, typename TObject>
    inline constexpr void OF_TYPE::bind(TObject *obj) {
        obj_ = (void*)(obj);
        calling_function_ = &functorCaller_<TObject, Functor>;
    }

    OF_TPL
    template<typename Functor, typename T, typename TObject>
    inline constexpr void OF_TYPE::bindT(TObject *obj) {
        obj_ = obj;
        calling_function_ = &functorCallerT_<TObject, Functor, T>;
    }

    OF_TPL
    inline TReturn OF_TYPE::operator()(Args... args)const {
        return calling_function_(this->obj_, std::forward<Args>(args)...);
    }

    OF_TPL
    inline constexpr TReturn OF_TYPE::callFor(void *obj, Args... args)const {
        return calling_function_(obj, std::forward<Args>(args)...);
    }

    OF_TPL
    inline void OF_TYPE::unbind() {
        obj_ = NULL;
        calling_function_ = &noOp_;
    }

    OF_TPL
    inline const void* OF_TYPE::getObjPtr()const {
        return obj_;
    }

    OF_TPL
    inline void*& OF_TYPE::accObjPtr() {
        return obj_;
    }

    OF_TPL
    inline bool OF_TYPE::operator==(const ObjFunc<TReturn(Args...)>& right)const {
        return obj_ == right.obj_ && calling_function_ == right.calling_function_;
    }

    OF_TPL
    template<typename TObject, typename Functor>
    inline TReturn OF_TYPE::functorCaller_(void *obj, Args &&... args) {
        // static
        TObject* o = static_cast<TObject*>(obj);
        return Functor::f(o, std::forward<Args>(args)...);
    }

    OF_TPL
    template<typename TObject, typename Functor, typename T>
    inline TReturn OF_TYPE::functorCallerT_(void *obj, Args &&... args) {
        // static
        TObject* o = static_cast<TObject*>(obj);
        return Functor::template f<T>(o, std::forward<Args>(args)...);
    }

    OF_TPL
    inline TReturn OF_TYPE::noOp_(void *obj, Args &&... args) {
        // static
        return TReturn();
    }

    template<typename Functor, typename TObject>
    inline constexpr auto createObjFunc(TObject *obj) {
        // static
        typename ObjFuncTypeFromFunctor<decltype(Functor::f)>::FuncT f;
        f.template bind<Functor>(obj);
        return std::move(f);
    }

    template<typename Functor, typename T, typename TObject>
    inline constexpr auto createObjFuncT(TObject *obj) {
        // static
        typename ObjFuncTypeFromFunctor<decltype(Functor::template f<T>)>::FuncT f;
        f.template bindT<Functor, T>(obj);
        return std::move(f);
    }
}

#undef OF_TPL
#undef OF_TYPE