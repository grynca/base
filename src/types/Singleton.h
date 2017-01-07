#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>

namespace grynca {

    template<class T>
    class Singleton {
    public:
        template <typename... ConstructionArgs>
        static T& create(ConstructionArgs&&... args);

        template <typename DerType, typename... ConstructionArgs>
        static DerType& createAs(ConstructionArgs&&... args);

        static T& get();
        static T* getptr();
    protected:
        Singleton(const Singleton&);
        Singleton(Singleton&&);
        Singleton& operator=(const Singleton&);
        Singleton();

        virtual ~Singleton();

        static T*& instance_();
    };


}

#include "Singleton.inl"
#endif //INGLETON_H
