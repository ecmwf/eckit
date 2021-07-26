/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/option/FactoryOption.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------
/// Build some generic factories to demonstrate functionality.
/// n.b. This does not worry about thread safety in the factory for the purpose of these unit tests.
///      Only one thread...

namespace {

/// A general base type, for objects to be built from.
class ObjectBase {
public:
    ObjectBase() {}
};

/// A general object factory.
class ObjectFactory {
    std::string name_;
    virtual ObjectBase* make() const = 0;
    static std::map<std::string, ObjectFactory*>& factory() {
        static std::map<std::string, ObjectFactory*> m;
        return m;
    }

protected:
    ObjectFactory(const std::string& nm) :
        name_(nm) { factory()[nm] = this; }
    virtual ~ObjectFactory() { factory().erase(name_); }

public:
    static void list(std::ostream& out) {
        const char* sep = "";
        for (std::map<std::string, ObjectFactory*>::const_iterator j = factory().begin(); j != factory().end(); ++j) {
            out << sep << (*j).first;
            sep = ", ";
        }
    }
    /*
    // unused
    static ObjectBase* build(const std::string& nm) { return factory()[nm]->make(); }
    */
};


/// Templated class to self-register, and build, objects
template <class T>
class ObjectBuilder : public ObjectFactory {
    virtual ObjectBase* make() const { return new T; }

public:
    ObjectBuilder(const std::string& name) :
        ObjectFactory(name) {}
};


/// Register the types
class Object1 : public ObjectBase {};
class Object2 : public ObjectBase {};

ObjectBuilder<Object1> factory1("obj1");
ObjectBuilder<Object2> factory2("obj2");

}  // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------

using namespace eckit::option;

CASE("test_eckit_option_factory_list") {

    FactoryOption<ObjectFactory> opt("arg1", "description");

    // Extract the output
    std::stringstream ss;
    ss << opt;
    std::string opt_str(ss.str());

    std::string cmp = "   --arg1=name (description)\n     Values are: obj1, obj2";
    EXPECT(opt_str == cmp);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
