#include "Singleton.h"
#include "../functions/debug.h"

namespace grynca {
    template <typename T>
    template <typename... ConstructionArgs>
    inline T& Singleton<T>::create(ConstructionArgs&&... args) {
    //static
        return *(instance_() = new T(std::forward<ConstructionArgs>(args)...));
    }

    template <typename T>
    template <typename DerType, typename... ConstructionArgs>
    inline DerType& Singleton<T>::createAs(ConstructionArgs&&... args) {
        //static
        return *(DerType*)(instance_() = new DerType(std::forward<ConstructionArgs>(args)...));
    }

    template <typename T>
    inline T& Singleton<T>::get() {
    //static
        ASSERT_M(instance_(), "Not created.");
        return *instance_();
    }

    template <typename T>
    inline T* Singleton<T>::getptr() {
    //static
        return instance_();
    }

    template <typename T>
    inline Singleton<T>::Singleton() {
        ASSERT_M(!instance_(), "Already created.");
    }

    template <typename T>
    inline Singleton<T>::~Singleton() {
        if (instance_()) {
            delete instance_();
            instance_()=NULL;
        }
    }

    template <typename T>
    inline T*& Singleton<T>::instance_() {
    //static
        static T* instance = NULL;
        return instance;
    }
}