/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <memory>

#include "eckit/log/Log.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Owned.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Translator.h"
#include "eckit/value/Properties.h"


#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

/// These tests are similar to the test for boost scoped_ptr and shared ptrs
/// This allows as in the future to drop out, our own home grown managed
/// ptr's in favour of the standards.

//----------------------------------------------------------------------------------------------------------------------

class Base0 {
public:
    typedef BuilderT0<Base0> builder_t;

    typedef std::shared_ptr<Base0> Ptr;

    static std::string className() { return "eckit_test.Base0"; }

    virtual ~Base0() {}
    virtual std::string foo() const = 0;
};

class A0 : public Base0 {
public:
    static std::string className() { return "eckit_test.A0"; }

    A0() :
        s1_("A.0") {}

    virtual std::string foo() const { return className() + "." + s1_; }

private:
    std::string s1_;
};

class B0 : public Base0 {
public:
    static std::string className() { return "eckit_test.B0"; }

    B0() :
        s2_("B.0") {}

    virtual std::string foo() const { return className() + "." + s2_; }

private:
    std::string s2_;
};

ConcreteBuilderT0<Base0, A0> builder_A0;
ConcreteBuilderT0<Base0, B0> builder_B0;

//----------------------------------------------------------------------------------------------------------------------

class Base1 : public Owned {
public:
    typedef BuilderT1<Base1> builder_t;
    typedef const Params& ARG1;

    typedef std::shared_ptr<Base1> Ptr;

    static std::string className() { return "eckit_test.Base1"; }

    virtual ~Base1() {}
    virtual std::string foo() const = 0;
};

class A1 : public Base1 {
public:
    static std::string className() { return "eckit_test.A1"; }

    A1(const Params& p) :
        s1_(p["mystr"].as<string>() + ".1") {}

    virtual std::string foo() const { return className() + "." + s1_; }

private:
    std::string s1_;
};

class B1 : public Base1 {
public:
    static std::string className() { return "eckit_test.B1"; }

    B1(const Params& p) :
        s2_(p["mystr"].as<string>() + ".2") {}

    virtual std::string foo() const { return className() + "." + s2_; }

private:
    std::string s2_;
};

ConcreteBuilderT1<Base1, A1> builder_A1;
ConcreteBuilderT1<Base1, B1> builder_B1;

//----------------------------------------------------------------------------------------------------------------------

class Base2 : public Owned {
public:
    typedef BuilderT2<Base2> builder_t;
    typedef std::string ARG1;
    typedef int ARG2;

    typedef std::shared_ptr<Base2> Ptr;

    static std::string className() { return "eckit_test.Base2"; }

    virtual ~Base2() {}
    virtual std::string foo() const = 0;
};

class A2 : public Base2 {
public:
    static std::string className() { return "eckit_test.A2"; }

    A2(std::string s, int i) :
        s1_(s + "." + Translator<int, string>()(i)) {}

    virtual std::string foo() const { return className() + "." + s1_; }

private:
    std::string s1_;
};

class B2 : public Base2 {
public:
    static std::string className() { return "eckit_test.B2"; }

    B2(std::string s, int i) :
        s2_(s + "." + Translator<int, string>()(i)) {}

    virtual std::string foo() const { return className() + "." + s2_; }

private:
    std::string s2_;
};

ConcreteBuilderT2<Base2, A2> builder_A2;
ConcreteBuilderT2<Base2, B2> builder_B2;

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_memory_factory_0") {
    //	std::cout << Factory<Base0>::instance() << std::endl;

    EXPECT(Factory<Base0>::build_type() == "eckit_test.Base0");

    EXPECT(Factory<Base0>::instance().size() == 2);

    EXPECT(Factory<Base0>::instance().exists("eckit_test.A0"));
    EXPECT(Factory<Base0>::instance().exists("eckit_test.B0"));

    EXPECT(Factory<Base0>::instance().get("eckit_test.A0").name() == "eckit_test.A0");
    EXPECT(Factory<Base0>::instance().get("eckit_test.B0").name() == "eckit_test.B0");

    Base0::Ptr p1(Factory<Base0>::instance().get("eckit_test.A0").create());
    Base0::Ptr p2(Factory<Base0>::instance().get("eckit_test.B0").create());

    EXPECT(p1);
    EXPECT(p2);

    EXPECT(p1->foo() == "eckit_test.A0.A.0");
    EXPECT(p2->foo() == "eckit_test.B0.B.0");
}

CASE("test_eckit_memory_factory_1") {
    //	std::cout << Factory<Base1>::instance() << std::endl;

    EXPECT(Factory<Base1>::build_type() == "eckit_test.Base1");

    EXPECT(Factory<Base1>::instance().size() == 2);

    EXPECT(Factory<Base1>::instance().exists("eckit_test.A1"));
    EXPECT(Factory<Base1>::instance().exists("eckit_test.B1"));

    EXPECT(Factory<Base1>::instance().get("eckit_test.A1").name() == "eckit_test.A1");
    EXPECT(Factory<Base1>::instance().get("eckit_test.B1").name() == "eckit_test.B1");

    Properties p;
    p.set("mystr", "lolo");

    Base1::Ptr p1(Factory<Base1>::instance().get("eckit_test.A1").create(Params(p)));
    Base1::Ptr p2(Factory<Base1>::instance().get("eckit_test.B1").create(Params(p)));

    EXPECT(p1);
    EXPECT(p2);

    EXPECT(p1->foo() == "eckit_test.A1.lolo.1");
    EXPECT(p2->foo() == "eckit_test.B1.lolo.2");
}

CASE("test_eckit_memory_factory_2") {
    //	std::cout << Factory<Base2>::instance() << std::endl;

    EXPECT(Factory<Base2>::build_type() == "eckit_test.Base2");

    EXPECT(Factory<Base2>::instance().size() == 2);

    EXPECT(Factory<Base2>::instance().exists("eckit_test.A2"));
    EXPECT(Factory<Base2>::instance().exists("eckit_test.B2"));

    EXPECT(Factory<Base2>::instance().get("eckit_test.A2").name() == "eckit_test.A2");
    EXPECT(Factory<Base2>::instance().get("eckit_test.B2").name() == "eckit_test.B2");

    string s("lolo");

    Base2::Ptr p1(Factory<Base2>::instance().get("eckit_test.A2").create(s, 42));
    Base2::Ptr p2(Factory<Base2>::instance().get("eckit_test.B2").create(s, 42));

    EXPECT(p1);
    EXPECT(p2);

    EXPECT(p1->foo() == "eckit_test.A2.lolo.42");
    EXPECT(p2->foo() == "eckit_test.B2.lolo.42");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
