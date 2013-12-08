#ifndef eckit_UnitTest_H
#define eckit_UnitTest_H

#include "eckit/runtime/Tool.h"
#include "eckit/log/Log.h"

class UnitTestRunner : public eckit::Tool {

    virtual void setUp();
    virtual void test();
    virtual void tearDown();

    virtual void run();

public:
    UnitTestRunner(int argc, char** argv);

};

typedef void (*test_proc_t)();

//=========================================

class OneTest {
    OneTest *next_;
    const char* n_;
    test_proc_t t_;
public:
    OneTest(const char* n, test_proc_t t);
    void test();
};

class TestsRunner : public UnitTestRunner {
    virtual void test();
public:
    TestsRunner(int argc, char** argv);
};

//=========================================

class SimpleTestRunner : public UnitTestRunner {
    test_proc_t t_;
    test_proc_t u_;
    test_proc_t d_;

    virtual void test();
    virtual void setUp();
    virtual void tearDown();

public:
    SimpleTestRunner(int argc, char** argv, test_proc_t t, test_proc_t u, test_proc_t d);

};

//=========================================


#define TEST(name) \
    static void name(); \
    static OneTest call##name(#name, &name); \
    static void name()

#define TEST_FIXTURE(type, name) \
    struct Test##type##name : public type {  void test(); }; \
    static void test##type##name() { Test##type##name().test(); } \
    static OneTest call##type##name("test"#type#name, &test##type##name); \
    void Test##type##name::test()



//=========================================

#define TEST_MAIN \
    int main(int c,char** v) { SimpleTestRunner x(c, v, &test, &setUp, &tearDown); x.start(); return 0; }

#define MANY_TESTS_MAIN \
    int main(int c,char** v) { TestsRunner x(c, v); x.start(); return 0; }

//=========================================


#define CHECK(expected) \
    ASSERT(expected)

#define CHECK_EQUAL(expected, actual) \
    ASSERT((expected) == (actual))


#define CHECK_ARRAY_EQUAL(expected, actual, count) \
    do { size_t n = count; for(size_t i=0;i<n;++i) CHECK_EQUAL(expected[i],actual[i]); } while(0)

#endif
