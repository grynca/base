#include "Singleton.h"

namespace grynca {
    template <typename T>
    template <typename... ConstructionArgs>
    inline T& Singleton<T>::create(ConstructionArgs&&... args) {
    //static
        return *(instance_() = new T(std::forward<ConstructionArgs>(args)...));
    }

    template <typename T>
    inline T& Singleton<T>::get() {
    //static
        assert(instance_() && "Not created.");
        return *instance_();
    }

    template <typename T>
    inline T* Singleton<T>::getptr() {
    //static
        return instance_();
    }

    template <typename T>
    inline Singleton<T>::Singleton() {
        assert(!instance_() && "Already created.");
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