#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>
#include "RefPtr.h"

namespace grynca {

    // when singleton is defaultly constructable you can use get() and it will be automatically constructed upon first get
    // if not you must use create/createAs and then getRef() or getPtr()

    template<class T>
    class Singleton {
    public:
        template <typename... ConstructionArgs>
        static T& create(ConstructionArgs&&... args);

        template <typename DerType, typename... ConstructionArgs>
        static DerType& createAs(ConstructionArgs&&... args);

        static T& get();
        static T& getRef();
        static T* getPtr();
    protected:
        template <typename... ConstructionArgs>
        static T& createIfNeeded_(ConstructionArgs&&... args);

        Singleton(const Singleton&);
        Singleton(Singleton&&);
        Singleton& operator=(const Singleton&);
        Singleton();

        virtual ~Singleton();

        static RefPtr<T>& instance_();
    };


}

#include "Singleton.inl"
#endif //INGLETON_H
