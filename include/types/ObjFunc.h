#ifndef OBJFUNC_H
#define OBJFUNC_H

template<typename signature>
class ObjFunc;

#define SIMPLE_FUNCTOR(NAME, BODY) \
    struct NAME { static void f BODY };

/*
 *  SIMPLE_FUNCTOR(DoStuff, (MyObj* obj, int arg) {
 *     return float(arg) + 0.5f;
 *  });
 *
 *  MyObj obj;
 *  ObjFunc<float(int)> f;
 *
 *  f.bind<DoStuff>(&obj);
 *  std::cout << (f(10) == 10.5f) << std::endl;
 *
 * can be also bound to NULL in that case first param of functor function will also be NULL
 */


template<typename TReturn, typename ...Args>
class ObjFunc<TReturn(Args...)> {
public:
    template<typename Functor, typename TObject = void>
    static ObjFunc<TReturn(Args...)> create(TObject *obj = NULL) {
        ObjFunc<TReturn(Args...)> f;
        f.bind<Functor>(obj);
        return std::move(f);
    }

    template<typename Functor, typename TObject = void>
    void bind(TObject *obj = NULL){
        this->obj = obj;
        this->callingFunction = &methodCaller<TObject, Functor>;
    }

    TReturn operator()(Args... args)const {
        return callingFunction(this->obj, std::forward<Args>(args)...);
    }
private:
    void *obj;
    TReturn (*callingFunction)(void *obj, Args&&...);

    template<typename TObject, typename Functor>
    static TReturn methodCaller(void *obj, Args&&... args) {
        TObject* o = static_cast<TObject*>(obj);
        return Functor::f(o, std::forward<Args>(args)...);
    }
};

#endif //OBJFUNC_H
