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
    TestParams() {
        dispatch_["foo"] = &TestParams::getFoo;
    }
    TestParams( Stream& ) {}

private:
    Params::value_t getFoo( const Params::key_t& key ) const {
        return Params::value_t("bar");
    }
};

//-----------------------------------------------------------------------------

struct AnyKeyParams {
    AnyKeyParams() {}
    AnyKeyParams( Stream& ) {}
    static const char* className() { return "AnyKeyParams"; }
};

Params::value_t get( const AnyKeyParams&, const Params::key_t& )
{
    return Params::value_t("foo");
}

void print( const AnyKeyParams&, std::ostream& ) {}
void encode( const AnyKeyParams&, Stream& ) {}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_value_params )

BOOST_AUTO_TEST_CASE( test_value_params_keys )
{
    BOOST_TEST_MESSAGE("Test keys present in Properties");

    Params p(Properties()
             .set("bool", true)
             .set("int", imax)
             .set("unsigned int", uimax)
             .set("long long", llmax)
             .set("unsigned long long", ullmax)
             .set("double", dmax)
             .set("string", "foo")
             .set("Length", Length(42))
             .set("Date", Date(2015, 2, 1))
             .set("PathName", PathName("/var/tmp"))
             );
    BOOST_TEST_MESSAGE("Params: " << p);
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

BOOST_AUTO_TEST_CASE( test_value_params )
{
    BOOST_TEST_MESSAGE("Initialize Properties");

    Params p(Properties()
             .set("bool", true)
             .set("int", imax)
             .set("unsigned int", uimax)
             .set("long long", llmax)
             .set("unsigned long long", ullmax)
             .set("double", dmax)
             .set("string", "foo")
             .set("Length", Length(42))
             .set("Date", Date(2015, 2, 1))
             .set("PathName", PathName("/var/tmp"))
             );
    BOOST_TEST_MESSAGE("Params: " << p);
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

BOOST_AUTO_TEST_CASE( test_composite_params )
{
    BOOST_TEST_MESSAGE("Initialize CompositeParams from Properties");

    Params cp(CompositeParams()
              .push_back(Params(Properties().set("bool", true)))
              .push_back(Params(Properties().set("int", imax)))
              .push_back(Params(Properties().set("unsigned int", uimax)))
              .push_back(Params(Properties().set("long long", llmax)))
              .push_back(Params(Properties().set("unsigned long long", ullmax)))
              .push_back(Params(Properties().set("double", dmax)))
              .push_back(Params(Properties().set("string", "foo")))
              .push_back(Params(Properties().set("Length", Length(42))))
              .push_back(Params(Properties().set("Date", Date(2015, 2, 1))))
              .push_back(Params(Properties().set("PathName", PathName("/var/tmp")))));

    BOOST_TEST_MESSAGE("Params: " << cp);
    BOOST_CHECK_EQUAL((bool)cp["bool"], true);
    BOOST_CHECK_EQUAL((int)cp["int"], imax);
    BOOST_CHECK_EQUAL((unsigned int)cp["unsigned int"], uimax);
    BOOST_CHECK_EQUAL((long long)cp["long long"], llmax);
    BOOST_CHECK_EQUAL((unsigned long long)cp["unsigned long long"], ullmax);
    BOOST_CHECK_EQUAL((double)cp["double"], dmax);
    BOOST_CHECK_EQUAL(cp["string"], "foo");
    BOOST_CHECK_EQUAL(cp["Length"], Length(42));
    BOOST_CHECK(cp["Date"].compare(Date(2015, 2, 1))); // FIXME: equality check fails
    BOOST_CHECK_EQUAL(cp["PathName"], PathName("/var/tmp"));
}

BOOST_AUTO_TEST_CASE( test_composite_params_list )
{
    BOOST_TEST_MESSAGE("Initialize CompositeParams from list of Properties");

    Params::List l;
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

    Params params( (CompositeParams(l)) ); // C++ Most Vexing Parse
    BOOST_TEST_MESSAGE("Params: " << params);
    BOOST_CHECK_EQUAL((bool)params["bool"], true);
    BOOST_CHECK_EQUAL((int)params["int"], imax);
    BOOST_CHECK_EQUAL((unsigned int)params["unsigned int"], uimax);
    BOOST_CHECK_EQUAL((long long)params["long long"], llmax);
    BOOST_CHECK_EQUAL((unsigned long long)params["unsigned long long"], ullmax);
    BOOST_CHECK_EQUAL((double)params["double"], dmax);
    BOOST_CHECK_EQUAL(params["string"], "foo");
    BOOST_CHECK_EQUAL(params["Length"], Length(42));
    BOOST_CHECK(params["Date"].compare(Date(2015, 2, 1))); // FIXME: equality check fails
    BOOST_CHECK_EQUAL(params["PathName"], PathName("/var/tmp"));
}

BOOST_AUTO_TEST_CASE( test_scope_params )
{
    Params sp(ScopeParams("scope", Params(Properties().set("foo", "bar"))));
    BOOST_CHECK( sp.has("scope.foo") );
    BOOST_CHECK( !sp.has("foo") );
    BOOST_CHECK_EQUAL( sp["scope.foo"], "bar" );
    BOOST_CHECK_THROW( sp["foo"], BadParameter );
}

BOOST_AUTO_TEST_CASE( test_composite_scope_params )
{
    Params user(Properties().set("resol", 100));
    Params def(Properties().set("resol", 200));

    Params p(CompositeParams()
             .push_back(Params(ScopeParams("user", user)))
             .push_back(Params(ScopeParams("default", def))));

    BOOST_CHECK( p.has("user.resol") );
    BOOST_CHECK( p.has("default.resol") );
    BOOST_CHECK( !p.has("resol") );
    BOOST_CHECK_EQUAL( (uint)p["user.resol"], 100 );
    BOOST_CHECK_EQUAL( (uint)p["default.resol"], 200 );
}

BOOST_AUTO_TEST_CASE( test_dispatch_params )
{
    Params p( (TestParams()) ); // C++ Most Vexing Parse
    BOOST_CHECK( p.has("foo") );
    BOOST_CHECK( !p.has("bar") );
    BOOST_CHECK_EQUAL( p["foo"], "bar" );
}

BOOST_AUTO_TEST_CASE( test_custom_params )
{
    Params p( (AnyKeyParams()) ); // C++ Most Vexing Parse
    BOOST_CHECK( p.has("foo") );
    BOOST_CHECK( p.has("bar") );
    BOOST_CHECK_EQUAL( p["foo"], "foo" );
    BOOST_CHECK_EQUAL( p["bar"], "foo" );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
