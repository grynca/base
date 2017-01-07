#ifndef MYDOMAIN_H
#define MYDOMAIN_H

#define RAND_DBL() ((f64)rand()/RAND_MAX)

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
public:

    static fast_vector<MyStuffVariant> generateN(u32 n) {
        fast_vector<MyStuffVariant> rvar;
        rvar.reserve(n);
        for (size_t i=0; i<n; ++i) {
            rvar.push_back(MyStuffVariant());
            switch (rand()%3) {
                case 0:
                    rvar.back().set<MyStuff>();
                    break;
                case 1:
                    rvar.back().set<MyStuffA>();
                    break;
                case 2:
                    rvar.back().set<MyStuffB>();
                    break;
            }
        }
        return rvar;
    }
};

namespace props {
    DECLARE_PROP(a);
    DECLARE_PROP(b);
    DECLARE_PROP(c);
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
