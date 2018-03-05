#ifndef OBJFUNC_H
#define OBJFUNC_H

namespace grynca {
    template<typename signature>
    class ObjFunc;
    template<typename signature>
    class ObjFuncTypeFromFunctor;

#define SIMPLE_FUNCTOR(NAME, BODY) \
    struct NAME { \
        static auto f BODY \
    };

// templated functor with one template argument T
#define TEMPLATED_FUNCTOR(NAME, BODY) \
    struct NAME { \
        template <typename T> static auto f BODY \
    };

// this creates ObjFunc bound to obj's member function FNAME
// ... are argument types
// e.g:
// auto fff = MEMBER_OBJ_FUNC(&obj, foo_method, int, f32);
#define MEMBER_OBJ_FUNC(obj_ptr, FNAME, ...) \
    [obj_ptr]() \
    { \
        typedef decltype(obj_ptr) TMP_PTRT; \
        struct TMP_F { static auto f (TMP_PTRT obj, MAKE_PARAMS(__VA_ARGS__)) { return obj->FNAME(MAKE_ARGS(__VA_ARGS__)); } };   \
        return std::move(createObjFunc<TMP_F>(obj_ptr)); \
    }()

/*
 *  BINDING TO OBJECT
 *  -----------------
 *
 *  struct MyObj {
 *     f32 foo(int a) {
 *        return 1.0f/a;
 *     }
 *  };
 *
 *  SIMPLE_FUNCTOR(DoStuffWithObj, (MyObj* obj, int arg) {
 *     return obj->foo(arg);
 *  });
 *
 *  MyObj obj;
 *  ObjFunc<float(int)> f;
 *  f.bind<DoStuffWithObj>(&obj);
 *
 *  // alternatively
 *  // auto f = createObjFunc<DoStuffWithObj>(&obj);
 *
 *  std::cout << (f(10) == 0.1f) << std::endl;
 *
 * BINDING TO TYPE
 * ---------------
 *  SIMPLE_FUNCTOR(DoStuffWithType, (MyType*, int arg) {
 *     return MyType::something(arg);
 *  });
 *
 * f.bind<DoStuffWithType, MyType>();
 *
 * WITHOUT TYPE, OBJECT
 * --------------------
 *  SIMPLE_FUNCTOR(DoStuff, (void*, int arg) {
 *     return arg;
 *  });
 *
 */

    template <typename TReturn, typename Arg1, typename ...Args>
    class ObjFuncTypeFromFunctor<TReturn(Arg1, Args...)> {
    public:
        typedef Arg1 ObjT;
        typedef ObjFunc<TReturn(Args...)> FuncT;
    };


    template<typename TReturn, typename ...Args>
    class ObjFunc<TReturn(Args...)> {
    public:
        constexpr ObjFunc();

        template<typename Functor, typename TObject = void>
        static constexpr ObjFunc<TReturn(Args...)> create(TObject *obj = NULL);

        template<typename Functor, typename T, typename TObject = void>
        static constexpr ObjFunc<TReturn(Args...)> createT(TObject *obj = NULL);        // for templated Functor

        template<typename Functor, typename TObject = void>
        constexpr void bind(TObject *obj = NULL);

        template<typename Functor, typename T, typename TObject = void>
        constexpr void bindT(TObject *obj = NULL);      // for templated Functor

        // standard calling, calls functor for bound object
        TReturn operator()(Args... args)const;

        // if you want to call for another object (make sure that object is of correct type that Functor expects!)
        constexpr TReturn callFor(void *obj, Args... args)const;

        // binds to no-op function
        void unbind();

        const void* getObjPtr()const;
        void*& accObjPtr();

        bool operator==(const ObjFunc<TReturn(Args...)>& right)const;
    private:
        void *obj_;
        TReturn (*calling_function_)(void *obj, Args&&...);     // binds to static functor

        // these should be inlined so only 1 func call is performed at runtime
        template<typename TObject, typename Functor>
        static TReturn functorCaller_(void *obj, Args &&... args);

        template<typename TObject, typename Functor, typename T>
        static TReturn functorCallerT_(void *obj, Args &&... args);     // for templated Functor

        static TReturn noOp_(void *obj, Args &&... args);
    };

    // simple creation from Functor (without need to specify ObjFunc type)
    template<typename Functor, typename TObject = void>
    static constexpr auto createObjFunc(TObject *obj = NULL);

    template<typename Functor, typename T, typename TObject = void>
    static constexpr auto createObjFuncT(TObject *obj = NULL);       // for templated Functor
}

#include "ObjFunc.inl"
#endif //OBJFUNC_H
