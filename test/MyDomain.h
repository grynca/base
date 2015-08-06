#ifndef MYDOMAIN_H
#define MYDOMAIN_H

#define RAND_DBL() ((double)rand()/RAND_MAX)

class MyStuff {
public:
    MyStuff() : a(rand()) {}
    virtual ~MyStuff() {}

    virtual void dostuff() { a -= rand(); }

    int a;
};

class MyStuffA : public MyStuff {
public:
    MyStuffA() : b(RAND_DBL()) {}
    virtual ~MyStuffA() {}

    virtual void dostuff() { b += RAND_DBL(); }

    double b;
    float c;
};

class MyStuffB : public MyStuff {
public:
    MyStuffB() : c(RAND_DBL()) {}
    virtual ~MyStuffB() {}

    virtual void dostuff() { c *= RAND_DBL(); }

    double c;
};

typedef TypesPack<
        MyStuff,
        MyStuffA,
        MyStuffB
    > StuffTypes;

class MyStuffVariant : public Variant<StuffTypes> {
public:
};

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
