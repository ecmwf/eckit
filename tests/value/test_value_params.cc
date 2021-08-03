/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#include <sys/types.h>
#include <limits>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/value/CompositeParams.h"
#include "eckit/value/DispatchParams.h"
#include "eckit/value/Params.h"
#include "eckit/value/Properties.h"
#include "eckit/value/ScopeParams.h"

#include "eckit/testing/Test.h"

#include "AnyKeyParams.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

//----------------------------------------------------------------------------------------------------------------------

namespace eckit_test {

const int imax                  = numeric_limits<int>::max();
const unsigned int uimax        = numeric_limits<unsigned int>::max();
const long long llmax           = numeric_limits<long long>::max();
const unsigned long long ullmax = numeric_limits<unsigned long long>::max();
const double dmax               = numeric_limits<double>::max();

//----------------------------------------------------------------------------------------------------------------------

class TestParams : public DispatchParams<TestParams> {
public:
    TestParams(const std::string& payload) :
        payload_(payload) { dispatch_["foo"] = &TestParams::getPayload; }
    TestParams(Stream& s) {
        dispatch_["foo"] = &TestParams::getPayload;
        s >> payload_;
    }

private:
    Params::value_t getPayload(const Params::key_t& key) const { return payload_; }

    friend void encode(const TestParams&, Stream&);

    string payload_;
};

void encode(const TestParams& p, Stream& s) {
    s << p.payload_;
}

Params::Factory<TestParams> testParamsFactory;

//----------------------------------------------------------------------------------------------------------------------

struct PropertiesFixture {
    PropertiesFixture() :
        p(Properties()
              .set("bool", true)
              .set("int", imax)
              .set("unsigned int", uimax)
              .set("long long", llmax)
              .set("unsigned long long", ullmax)
              .set("double", dmax)
              .set("string", "foo")
              .set("Length", Length(42))
              .set("Date", Date(2015, 2, 1))
              .set("PathName", PathName("/var/tmp"))) {}

    Params p;
};

struct CompositeParamsFixture {
    CompositeParamsFixture() :
        p(CompositeParams()
              .push_back(Params(Properties().set("bool", true)))
              .push_back(Params(Properties().set("int", imax)))
              .push_back(Params(Properties().set("unsigned int", uimax)))
              .push_back(Params(Properties().set("long long", llmax)))
              .push_back(Params(Properties().set("unsigned long long", ullmax)))
              .push_back(Params(Properties().set("double", dmax)))
              .push_back(Params(Properties().set("string", "foo")))
              .push_back(Params(Properties().set("Length", Length(42))))
              .push_back(Params(Properties().set("Date", Date(2015, 2, 1))))
              .push_back(Params(Properties().set("PathName", PathName("/var/tmp"))))) {}

    Params p;
};

struct ListParamsFixture {
    ListParamsFixture() :
        p(CompositeParams(ListParamsFixture::pl())) {}

    static Params::List& pl() {
        static Params::List l;
        if (!l.size()) {
            l.push_back(Params(Properties().set("bool", true)));
            l.push_back(Params(Properties().set("int", imax)));
            l.push_back(Params(Properties().set("unsigned int", uimax)));
            l.push_back(Params(Properties().set("long long", llmax)));
            l.push_back(Params(Properties().set("unsigned long long", ullmax)));
            l.push_back(Params(Properties().set("double", dmax)));
            l.push_back(Params(Properties().set("string", "foo")));
            l.push_back(Params(Properties().set("Length", Length(42))));
            l.push_back(Params(Properties().set("Date", Date(2015, 2, 1))));
            l.push_back(Params(Properties().set("PathName", PathName("/var/tmp"))));
        }
        return l;
    }

    Params p;
};

struct ScopedParamsFixture {
    ScopedParamsFixture() :
        p(ScopeParams("scope", Params(Properties().set("foo", "bar")))) {}

    Params p;
};

struct CompositeScopedParamsFixture {
    CompositeScopedParamsFixture() :
        p(CompositeParams()
              .push_back(Params(ScopeParams("user", Params(Properties().set("resol", 100)))))
              .push_back(Params(ScopeParams("default", Params(Properties().set("resol", 200)))))) {}

    Params p;
};

struct DispatchParamsFixture {
    DispatchParamsFixture() :
        p(TestParams("bar")) {}

    Params p;
};

struct AnyKeyParamsFixture {
    AnyKeyParamsFixture() :
        p(AnyKeyParams("foo")) {}

    Params p;
};

//----------------------------------------------------------------------------------------------------------------------

void test_keys(const Params& p) {
    EXPECT(p.has("bool"));
    EXPECT(p.has("int"));
    EXPECT(p.has("unsigned int"));
    EXPECT(p.has("long long"));
    EXPECT(p.has("unsigned long long"));
    EXPECT(p.has("double"));
    EXPECT(p.has("string"));
    EXPECT(p.has("Length"));
    EXPECT(p.has("Date"));
    EXPECT(p.has("PathName"));
    EXPECT(!p.has("foo"));
    EXPECT_THROWS_AS(p["foo"], BadParameter);
}

void test_vals(const Params& p) {
    EXPECT((bool)p["bool"] == true);
    EXPECT((int)p["int"] == imax);
    EXPECT((unsigned int)p["unsigned int"] == uimax);
    EXPECT((long long)p["long long"] == llmax);
    EXPECT((unsigned long long)p["unsigned long long"] == ullmax);
    EXPECT((double)p["double"] == dmax);
    EXPECT(p["string"] == "foo");
    EXPECT(p["Length"] == Length(42));
    EXPECT(p["Date"] == (Date(2015, 2, 1)));
    EXPECT(p["PathName"] == PathName("/var/tmp"));
}

void test_scope(const Params& p) {
    EXPECT(p.has("scope.foo"));
    EXPECT(!p.has("foo"));
    EXPECT(p["scope.foo"] == "bar");
    EXPECT_THROWS_AS(p["foo"], BadParameter);
}

void test_composite_scope(const Params& p) {
    EXPECT(p.has("user.resol"));
    EXPECT(p.has("default.resol"));
    EXPECT(!p.has("resol"));
    EXPECT((uint)p["user.resol"] == 100);
    EXPECT((uint)p["default.resol"] == 200);
}

void test_dispatch(const Params& p) {
    EXPECT(p.has("foo"));
    EXPECT(!p.has("bar"));
    EXPECT(p["foo"] == "bar");
}

void test_custom(const Params& p) {
    EXPECT(p.has("foo"));
    EXPECT(p.has("bar"));
    EXPECT(p["foo"] == "foo");
    EXPECT(p["bar"] == "foo");
}

//----------------------------------------------------------------------------------------------------------------------

Params stream_to_from_file(const Params& p) {
    PathName filename    = PathName::unique("data");
    std::string filepath = filename.asString();
    {
        FileStream sout(filepath.c_str(), "w");
        auto c = closer(sout);
        sout << p;
    }
    {
        FileStream sin(filepath.c_str(), "r");
        auto c = closer(sin);
        return Params(Params::decode(sin));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_properties") {

    PropertiesFixture fix;

    SECTION("test_properties_params_init") {
        eckit::Log::info() << "Initialize Properties" << std::endl;
        eckit::Log::info() << "Params: " << fix.p << std::endl;
        test_keys(fix.p);
        test_vals(fix.p);
    }

    SECTION("test_properties_params_streaming") {
        eckit::Log::info() << "Stream Properties" << std::endl;
        eckit::Log::info() << "original: " << fix.p << std::endl;
        Params params = stream_to_from_file(fix.p);
        eckit::Log::info() << "streamed: " << params << std::endl;
        test_keys(params);
        test_vals(params);
    }
}

CASE("test_composite_params") {

    CompositeParamsFixture fix;

    SECTION("test_composite_params") {
        eckit::Log::info() << "Initialize CompositeParams from Properties" << std::endl;
        eckit::Log::info() << "Params: " << fix.p << std::endl;
        test_keys(fix.p);
        test_vals(fix.p);
    }

    SECTION("test_composite_params_streaming") {
        eckit::Log::info() << "Stream CompositeParams initialised from Properties" << std::endl;
        eckit::Log::info() << "original: " << fix.p << std::endl;
        Params params = stream_to_from_file(fix.p);
        eckit::Log::info() << "streamed: " << params << std::endl;
        test_keys(params);
        test_vals(params);
    }
}


CASE("test_composite_params_list") {

    ListParamsFixture fix;

    SECTION("test_composite_params_list_init") {
        eckit::Log::info() << "Initialize CompositeParams from list of Properties" << std::endl;
        eckit::Log::info() << "Params: " << fix.p << std::endl;
        test_keys(fix.p);
        test_vals(fix.p);
    }

    SECTION("test_composite_params_list_streaming") {
        eckit::Log::info() << "Stream CompositeParams initialised from list of Properties" << std::endl;
        eckit::Log::info() << "original: " << fix.p << std::endl;
        Params params = stream_to_from_file(fix.p);
        eckit::Log::info() << "streamed: " << params << std::endl;
        test_keys(params);
        test_vals(params);
    }
}

CASE("test_scoped_params") {

    ScopedParamsFixture fix;

    SECTION("test_scope_params_init") {
        eckit::Log::info() << "Initialize ScopedParams" << std::endl;
        eckit::Log::info() << "Params: " << fix.p << std::endl;
        test_scope(fix.p);
    }

    SECTION("test_scope_params_streaming") {
        eckit::Log::info() << "Stream ScopedParams" << std::endl;
        eckit::Log::info() << "original: " << fix.p << std::endl;
        Params params = stream_to_from_file(fix.p);
        eckit::Log::info() << "streamed: " << params << std::endl;
        test_scope(params);
    }
}

CASE("test_composite_scope_params") {

    CompositeScopedParamsFixture fix;

    SECTION("test_composite_scope_params_init") {
        eckit::Log::info() << "Initialize CompositeParams from ScopedParams" << std::endl;
        eckit::Log::info() << "Params: " << fix.p << std::endl;
        test_composite_scope(fix.p);
    }

    SECTION("test_composite_scope_params_streaming") {
        eckit::Log::info() << "Stream CompositeParams initialised from ScopedParams" << std::endl;
        eckit::Log::info() << "original: " << fix.p << std::endl;
        Params params = stream_to_from_file(fix.p);
        eckit::Log::info() << "streamed: " << params << std::endl;
        test_composite_scope(params);
    }
}

CASE("test_dispatch_params") {

    DispatchParamsFixture fix;

    SECTION("test_dispatch_params_init") {
        eckit::Log::info() << "Initialize DispatchParams" << std::endl;
        test_dispatch(fix.p);
    }

    SECTION("test_dispatch_params_streaming") {
        eckit::Log::info() << "Stream DispatchParams" << std::endl;
        Params params = stream_to_from_file(fix.p);
        test_dispatch(params);
    }
}

CASE("test_custom_params") {

    AnyKeyParamsFixture fix;

    SECTION("test_custom_params_init") {
        eckit::Log::info() << "Initialize custom AnyKeyParams" << std::endl;
        test_custom(fix.p);
    }

    SECTION("test_custom_params_streaming") {
        eckit::Log::info() << "Stream custom AnyKeyParams" << std::endl;
        Params params = stream_to_from_file(fix.p);
        test_custom(params);
    }
}

}  // namespace eckit_test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
