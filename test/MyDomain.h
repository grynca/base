#ifndef MYDOMAIN_H
#define MYDOMAIN_H

#define RAND_DBL() ((f64)rand()/RAND_MAX)

// fw
class MyStuff;

class MyStuffManager : public TightManager<MyStuff> {};

class MyStuff : public ManagedItem<MyStuffManager> {
public:
    MyStuff() : a(rand()) {}
    virtual ~MyStuff() {}

    virtual void dostuff() { a -= rand(); }
    virtual void ble()const {}

    void func() {}

    int a;
};

class MyStuffSorted {
public:
    MyStuffSorted(u32 sk) : id_(InvalidId()), sorting_key_(sk) {}

    u32 getId()const { return id_; }
    u32 getSortingKey()const { return sorting_key_; }

    void dostuff() { val_ -= rand(); }
private:
    friend class SortedArray<MyStuffSorted>;

    u32& accId_() { return id_; }
    void setLast_() { sorting_key_ = InvalidId(); }

    struct Comparator {
        // returns if i1 is before i2
        bool operator()(const MyStuffSorted& i1, const MyStuffSorted& i2) {
            return i1.getSortingKey() < i2.getSortingKey();
        }
    };

    u32 id_;
    u32 sorting_key_;
    int val_;
};

class MyStuffA : public MyStuff {
public:
    MyStuffA() : b(RAND_DBL()) {}
    virtual ~MyStuffA() {}

    virtual void dostuff() { b += RAND_DBL(); }

    static void funcStatic() {}

    f64 b;
    f32 c;
};

class MyStuffB : public MyStuff {
public:
    MyStuffB() : c(RAND_DBL()) {}
    virtual ~MyStuffB() {}

    virtual void dostuff() { c *= RAND_DBL(); }

    f64 c;
};

typedef TypesPack<
        MyStuff,
        MyStuffA,
        MyStuffB
    > StuffTypes;

class MyStuffVariant : public Variant<StuffTypes> {
};

namespace props {
    DECLARE_PROP_DATA(a);
    DECLARE_PROP_DATA(b);
    DECLARE_PROP_DATA(c);
    DECLARE_PROP_FUNC(func);
    DECLARE_PROP_FUNC(funcStatic);
}

class MyDomain {
public:
    static void init() {
        TypeInfoManager<MyDomain>::setTypeId<MyStuff>(0);
        TypeInfoManager<MyDomain>::setTypeId<MyStuffA>(1);
        TypeInfoManager<MyDomain>::setTypeId<MyStuffB>(2);
        TypeInfoManager<MyDomain>::setTypeId<MyStuffVariant>(3);
    }
};

#endif //MYDOMAIN_H
