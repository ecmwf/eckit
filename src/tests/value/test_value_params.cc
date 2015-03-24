/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_value_params

#include "ecbuild/boost_test_framework.h"

#include "eckit/value/CompositeParams.h"
#include "eckit/value/DispatchParams.h"
#include "eckit/value/Params.h"
#include "eckit/value/Properties.h"
#include "eckit/value/ScopeParams.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

const int imax = numeric_limits<int>::max();
const unsigned int uimax = numeric_limits<unsigned int>::max();
const long long llmax = numeric_limits<long long>::max();
const unsigned long long ullmax = numeric_limits<unsigned long long>::max();
const double dmax = numeric_limits<double>::max();

//-----------------------------------------------------------------------------

class TestParams : public DispatchParams<TestParams> {
public:
    TestParams(const std::string& payload) : payload_(payload) {
        dispatch_["foo"] = &TestParams::getPayload;
    }
    TestParams( Stream& s ) {
        dispatch_["foo"] = &TestParams::getPayload;
        s >> payload_;
    }

private:
    Params::value_t getPayload( const Params::key_t& key ) const {
        return payload_;
    }

    friend void encode( const TestParams&, Stream& );

    string payload_;
};

void encode( const TestParams& p, Stream& s )
{
    s << p.payload_;
}

Params::Factory<TestParams> testParamsFactory;

//-----------------------------------------------------------------------------

struct AnyKeyParams {
    AnyKeyParams(std::string payload) : payload_(payload) {}
    AnyKeyParams( Stream& s ) {
        s >> payload_;
    }
    static const char* className() { return "AnyKeyParams"; }
private:
    friend void encode( const AnyKeyParams&, Stream& );
    std::string payload_;
};

Params::value_t get( const AnyKeyParams&, const Params::key_t& )
{
    return Params::value_t("foo");
}

void print( const AnyKeyParams&, std::ostream& ) {}
void encode( const AnyKeyParams& p, Stream& s ) {
    s << p.payload_;
}

Params::Factory<AnyKeyParams> anyKeyParamsFactory;

//-----------------------------------------------------------------------------

struct PropertiesFixture
{
    PropertiesFixture()
      : p( Properties()
           .set("bool", true)
           .set("int", imax)
           .set("unsigned int", uimax)
           .set("long long", llmax)
           .set("unsigned long long", ullmax)
           .set("double", dmax)
           .set("string", "foo")
           .set("Length", Length(42))
           .set("Date", Date(2015, 2, 1))
           .set("PathName", PathName("/var/tmp")) ) {}

    Params p;
};

struct CompositeParamsFixture
{
    CompositeParamsFixture()
      : p( CompositeParams()
           .push_back(Params(Properties().set("bool", true)))
           .push_back(Params(Properties().set("int", imax)))
           .push_back(Params(Properties().set("unsigned int", uimax)))
           .push_back(Params(Properties().set("long long", llmax)))
           .push_back(Params(Properties().set("unsigned long long", ullmax)))
           .push_back(Params(Properties().set("double", dmax)))
           .push_back(Params(Properties().set("string", "foo")))
           .push_back(Params(Properties().set("Length", Length(42))))
           .push_back(Params(Properties().set("Date", Date(2015, 2, 1))))
           .push_back(Params(Properties().set("PathName", PathName("/var/tmp")))) ) {}

    Params p;
};

struct ListParamsFixture
{
    ListParamsFixture()
      : p( CompositeParams(ListParamsFixture::pl()) ) {}

    static Params::List& pl () {
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

struct ScopedParamsFixture
{
    ScopedParamsFixture()
      : p( ScopeParams("scope", Params(Properties().set("foo", "bar"))) ) {}

    Params p;
};

struct CompositeScopedParamsFixture
{
    CompositeScopedParamsFixture()
      : p( CompositeParams()
           .push_back(Params(ScopeParams("user",
                                         Params(Properties().set("resol", 100)))))
           .push_back(Params(ScopeParams("default",
                                         Params(Properties().set("resol", 200))))) ) {}

    Params p;
};

struct DispatchParamsFixture
{
    DispatchParamsFixture()
      : p( TestParams("bar") ) {}

    Params p;
};

struct AnyKeyParamsFixture
{
    AnyKeyParamsFixture()
      : p( AnyKeyParams("foo") ) {}

    Params p;
};

//-----------------------------------------------------------------------------

void test_keys(const Params& p)
{
    BOOST_CHECK( p.has("bool") );
    BOOST_CHECK( p.has("int") );
    BOOST_CHECK( p.has("unsigned int") );
    BOOST_CHECK( p.has("long long") );
    BOOST_CHECK( p.has("unsigned long long") );
    BOOST_CHECK( p.has("double") );
    BOOST_CHECK( p.has("string") );
    BOOST_CHECK( p.has("Length") );
    BOOST_CHECK( p.has("Date") );
    BOOST_CHECK( p.has("PathName") );
    BOOST_CHECK( !p.has("foo") );
    BOOST_CHECK_THROW( p["foo"], BadParameter );
}

void test_vals(const Params& p)
{
    BOOST_CHECK_EQUAL((bool)p["bool"], true);
    BOOST_CHECK_EQUAL((int)p["int"], imax);
    BOOST_CHECK_EQUAL((unsigned int)p["unsigned int"], uimax);
    BOOST_CHECK_EQUAL((long long)p["long long"], llmax);
    BOOST_CHECK_EQUAL((unsigned long long)p["unsigned long long"], ullmax);
    BOOST_CHECK_EQUAL((double)p["double"], dmax);
    BOOST_CHECK_EQUAL(p["string"], "foo");
    BOOST_CHECK_EQUAL(p["Length"], Length(42));
    BOOST_CHECK(p["Date"].compare(Date(2015, 2, 1))); // FIXME: equality check fails
    BOOST_CHECK_EQUAL(p["PathName"], PathName("/var/tmp"));
}

void test_scope(const Params& p)
{
    BOOST_CHECK( p.has("scope.foo") );
    BOOST_CHECK( !p.has("foo") );
    BOOST_CHECK_EQUAL( p["scope.foo"], "bar" );
    BOOST_CHECK_THROW( p["foo"], BadParameter );
}

void test_composite_scope(const Params& p)
{
    BOOST_CHECK( p.has("user.resol") );
    BOOST_CHECK( p.has("default.resol") );
    BOOST_CHECK( !p.has("resol") );
    BOOST_CHECK_EQUAL( (uint)p["user.resol"], 100 );
    BOOST_CHECK_EQUAL( (uint)p["default.resol"], 200 );
}

void test_dispatch(const Params& p)
{
    BOOST_CHECK( p.has("foo") );
    BOOST_CHECK( !p.has("bar") );
    BOOST_CHECK_EQUAL( p["foo"], "bar" );
}

void test_custom(const Params& p)
{
    BOOST_CHECK( p.has("foo") );
    BOOST_CHECK( p.has("bar") );
    BOOST_CHECK_EQUAL( p["foo"], "foo" );
    BOOST_CHECK_EQUAL( p["bar"], "foo" );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_value_params )

BOOST_FIXTURE_TEST_CASE( test_properties_params, PropertiesFixture )
{
    BOOST_TEST_MESSAGE("Initialize Properties");
    BOOST_TEST_MESSAGE("Params: " << p);
    test_keys(p);
    test_vals(p);
}

BOOST_FIXTURE_TEST_CASE( test_composite_params, CompositeParamsFixture )
{
    BOOST_TEST_MESSAGE("Initialize CompositeParams from Properties");
    BOOST_TEST_MESSAGE("Params: " << p);
    test_keys(p);
    test_vals(p);
}

BOOST_FIXTURE_TEST_CASE( test_composite_params_list, ListParamsFixture )
{
    BOOST_TEST_MESSAGE("Initialize CompositeParams from list of Properties");
    BOOST_TEST_MESSAGE("Params: " << p);
    test_keys(p);
    test_vals(p);
}

BOOST_FIXTURE_TEST_CASE( test_scope_params, ScopedParamsFixture )
{
    BOOST_TEST_MESSAGE("Initialize ScopedParams");
    BOOST_TEST_MESSAGE("Params: " << p);
    test_scope(p);
}

BOOST_FIXTURE_TEST_CASE( test_composite_scope_params, CompositeScopedParamsFixture )
{
    BOOST_TEST_MESSAGE("Initialize CompositeParams from ScopedParams");
    BOOST_TEST_MESSAGE("Params: " << p);
    test_composite_scope(p);
}

BOOST_FIXTURE_TEST_CASE( test_dispatch_params, DispatchParamsFixture )
{
    BOOST_TEST_MESSAGE("Initialize DispatchParams");
    test_dispatch(p);
}

BOOST_FIXTURE_TEST_CASE( test_custom_params, AnyKeyParamsFixture )
{
    BOOST_TEST_MESSAGE("Initialize custom AnyKeyParams");
    test_custom(p);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
