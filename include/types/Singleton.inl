#include "Singleton.h"
#include "../functions/debug.h"

namespace grynca {
    template <typename T>
    template <typename... ConstructionArgs>
    inline T& Singleton<T>::create(ConstructionArgs&&... args) {
        //static
        instance_() = new T(std::forward<ConstructionArgs>(args)...);
        return instance_().get();
    }

    template <typename T>
    template <typename DerType, typename... ConstructionArgs>
    inline DerType& Singleton<T>::createAs(ConstructionArgs&&... args) {
        //static
        instance_() = new DerType(std::forward<ConstructionArgs>(args)...);
        return *(DerType*)(instance_().getPtr());
    }

    template <typename T>
    inline void Singleton<T>::destroy() {
        T*& ptr = instance_().accPtr();
        delete ptr;
        ptr = NULL;
    }

    template <typename T>
    inline T& Singleton<T>::get() {
        //static
        static T& me = createIfNeeded_();
        return me;
    }

    template <typename T>
    inline T& Singleton<T>::getRef() {
    //static
        ASSERT_M(instance_().getPtr(), "Not created.");
        return instance_().get();
    }

    template <typename T>
    inline const T* Singleton<T>::getPtr() {
    //static
        return instance_().getPtr();
    }

    template <typename T>
    inline T*& Singleton<T>::accPtr() {
    // static
        return instance_().accPtr();
    }

    template <typename T>
    template <typename... ConstructionArgs>
    inline T& Singleton<T>::createIfNeeded_(ConstructionArgs&&... args) {
        // static
        T* ptr = accPtr();
        if (ptr)
            return *ptr;
        //else
        return create(std::forward<ConstructionArgs>(args)...);
    }

    template <typename T>
    inline Singleton<T>::Singleton() {
    }

    template <typename T>
    inline Singleton<T>::~Singleton() {
    }

    template <typename T>
    inline RefPtr<T>& Singleton<T>::instance_() {
    //static
        static RefPtr<T> instance = NULL;
        return instance;
    }
}