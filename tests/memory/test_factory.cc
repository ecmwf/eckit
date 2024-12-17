/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <memory>
#include <string>

#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"
#include "eckit/memory/Owned.h"
#include "eckit/testing/Test.h"
#include "eckit/value/Properties.h"


namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

class Base0 {
public:
    using builder_t = BuilderT0<Base0>;

    static std::string className() { return "eckit_test.Base0"; }

    virtual ~Base0() = default;

    virtual std::string foo() const = 0;
};

class A0 : public Base0 {
public:
    static std::string className() { return "eckit_test.A0"; }

    A0() : s1_("A.0") {}

    std::string foo() const override { return className() + "." + s1_; }

private:
    std::string s1_;
};

class B0 : public Base0 {
public:
    static std::string className() { return "eckit_test.B0"; }

    B0() : s2_("B.0") {}

    std::string foo() const override { return className() + "." + s2_; }

private:
    std::string s2_;
};

static const ConcreteBuilderT0<Base0, A0> builder_A0;
static const ConcreteBuilderT0<Base0, B0> builder_B0;

//----------------------------------------------------------------------------------------------------------------------

class Base1 : public Owned {
public:
    using builder_t = BuilderT1<Base1>;
    using ARG1      = const Properties&;

    static std::string className() { return "eckit_test.Base1"; }

    virtual ~Base1() = default;

    virtual std::string foo() const = 0;
};

class A1 : public Base1 {
public:
    static std::string className() { return "eckit_test.A1"; }

    explicit A1(const Properties& p) : s1_(p["mystr"].as<std::string>() + ".1") {}

    std::string foo() const override { return className() + "." + s1_; }

private:
    std::string s1_;
};

class B1 : public Base1 {
public:
    static std::string className() { return "eckit_test.B1"; }

    explicit B1(const Properties& p) : s2_(p["mystr"].as<std::string>() + ".2") {}

    std::string foo() const override { return className() + "." + s2_; }

private:
    std::string s2_;
};

static const ConcreteBuilderT1<Base1, A1> builder_A1;
static const ConcreteBuilderT1<Base1, B1> builder_B1;

//----------------------------------------------------------------------------------------------------------------------

class Base2 : public Owned {
public:
    using builder_t = BuilderT2<Base2>;
    using ARG1      = std::string;
    using ARG2      = int;

    static std::string className() { return "eckit_test.Base2"; }

    virtual ~Base2() = default;

    virtual std::string foo() const = 0;
};

class A2 : public Base2 {
public:
    static std::string className() { return "eckit_test.A2"; }

    A2(std::string s, int i) : s1_(s + "." + std::to_string(i)) {}

    std::string foo() const override { return className() + "." + s1_; }

private:
    std::string s1_;
};

class B2 : public Base2 {
public:
    static std::string className() { return "eckit_test.B2"; }

    B2(std::string s, int i) : s2_(s + "." + std::to_string(i)) {}

    std::string foo() const override { return className() + "." + s2_; }

private:
    std::string s2_;
};

static const ConcreteBuilderT2<Base2, A2> builder_A2;
static const ConcreteBuilderT2<Base2, B2> builder_B2_1;
static const ConcreteBuilderT2<Base2, B2> builder_B2_2("eckit_test.B2.x");

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_memory_factory_0") {
    EXPECT(Factory<Base0>::build_type() == "eckit_test.Base0");
    const auto& factory = Factory<Base0>::instance();

    EXPECT(factory.size() == 2);

    EXPECT(factory.exists("eckit_test.A0"));
    EXPECT(factory.exists("eckit_test.B0"));

    EXPECT(factory.get("eckit_test.A0").name() == "eckit_test.A0");
    EXPECT(factory.get("eckit_test.B0").name() == "eckit_test.B0");

    std::unique_ptr<Base0> p1(factory.get("eckit_test.A0").create());
    std::unique_ptr<Base0> p2(factory.get("eckit_test.B0").create());

    EXPECT(p1);
    EXPECT(p2);

    EXPECT(p1->foo() == "eckit_test.A0.A.0");
    EXPECT(p2->foo() == "eckit_test.B0.B.0");
}

CASE("test_eckit_memory_factory_1") {
    EXPECT(Factory<Base1>::build_type() == "eckit_test.Base1");
    const auto& factory = Factory<Base1>::instance();

    EXPECT(factory.size() == 2);

    EXPECT(factory.exists("eckit_test.A1"));
    EXPECT(factory.exists("eckit_test.B1"));

    EXPECT(factory.get("eckit_test.A1").name() == "eckit_test.A1");
    EXPECT(factory.get("eckit_test.B1").name() == "eckit_test.B1");

    Properties p;
    p.set("mystr", "lolo");

    std::unique_ptr<Base1> p1(factory.get("eckit_test.A1").create(p));
    std::unique_ptr<Base1> p2(factory.get("eckit_test.B1").create(p));

    EXPECT(p1);
    EXPECT(p2);

    EXPECT(p1->foo() == "eckit_test.A1.lolo.1");
    EXPECT(p2->foo() == "eckit_test.B1.lolo.2");
}

CASE("test_eckit_memory_factory_2") {
    EXPECT(Factory<Base2>::build_type() == "eckit_test.Base2");
    const auto& factory = Factory<Base2>::instance();

    EXPECT(factory.size() == 3);

    EXPECT(factory.exists("eckit_test.A2"));
    EXPECT(factory.exists("eckit_test.B2"));

    EXPECT(factory.get("eckit_test.A2").name() == "eckit_test.A2");
    EXPECT(factory.get("eckit_test.B2").name() == "eckit_test.B2");

    std::string s("lolo");

    std::unique_ptr<Base2> p1(factory.get("eckit_test.A2").create(s, 42));
    std::unique_ptr<Base2> p2(factory.get("eckit_test.B2").create(s, 42));
    std::unique_ptr<Base2> p3(factory.get("eckit_test.B2.x").create(s, -42));

    EXPECT(p1);
    EXPECT(p2);
    EXPECT(p3);

    EXPECT(p1->foo() == "eckit_test.A2.lolo.42");
    EXPECT(p2->foo() == "eckit_test.B2.lolo.42");
    EXPECT(p3->foo() == "eckit_test.B2.lolo.-42");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
