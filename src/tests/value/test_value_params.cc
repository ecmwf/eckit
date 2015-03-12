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

#include "eckit/value/Params.h"

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

private:
    Params::value_t getFoo( const Params::key_t& key ) const {
        return Params::value_t("bar");
    }
};

//-----------------------------------------------------------------------------

class AnyKeyParams {
    friend Params::value_t get( const AnyKeyParams&, const Params::key_t& );
    friend void print( const AnyKeyParams&, std::ostream& );
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
    BOOST_TEST_MESSAGE("Test keys present in ValueParams");
    ValueParams p;
    p.set("bool", true);
    p.set("int", imax);
    p.set("unsigned int", uimax);
    p.set("long long", llmax);
    p.set("unsigned long long", ullmax);
    p.set("double", dmax);
    p.set("string", "foo");
    p.set("Length", Length(42));
    p.set("Date", Date(2015, 2, 1));
    p.set("PathName", PathName("/var/tmp"));

    Params params(p);
    BOOST_TEST_MESSAGE("Params: " << params);
    BOOST_CHECK( params.has("bool") );
    BOOST_CHECK( params.has("int") );
    BOOST_CHECK( params.has("unsigned int") );
    BOOST_CHECK( params.has("long long") );
    BOOST_CHECK( params.has("unsigned long long") );
    BOOST_CHECK( params.has("double") );
    BOOST_CHECK( params.has("string") );
    BOOST_CHECK( params.has("Length") );
    BOOST_CHECK( params.has("Date") );
    BOOST_CHECK( params.has("PathName") );
    BOOST_CHECK( !params.has("foo") );
    BOOST_CHECK_THROW( params["foo"], BadParameter );
}

BOOST_AUTO_TEST_CASE( test_value_params )
{
    BOOST_TEST_MESSAGE("Initialize ValueParams");
    ValueParams p;
    p.set("bool", true);
    p.set("int", imax);
    p.set("unsigned int", uimax);
    p.set("long long", llmax);
    p.set("unsigned long long", ullmax);
    p.set("double", dmax);
    p.set("string", "foo");
    p.set("Length", Length(42));
    p.set("Date", Date(2015, 2, 1));
    p.set("PathName", PathName("/var/tmp"));

    Params params(p);
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

BOOST_AUTO_TEST_CASE( test_value_params_from_properties )
{
    BOOST_TEST_MESSAGE("Initialize ValueParams from Properties");
    Properties p;
    p.set("bool", true);
    p.set("int", imax);
    p.set("unsigned int", uimax);
    p.set("long long", llmax);
    p.set("unsigned long long", ullmax);
    p.set("double", dmax);
    p.set("string", "foo");
    p.set("Length", Length(42));
    p.set("Date", Date(2015, 2, 1));
    p.set("PathName", PathName("/var/tmp"));

    Params params( (ValueParams(p)) ); // C++ Most Vexing Parse
    BOOST_TEST_MESSAGE("Params: " << params);
    BOOST_CHECK_EQUAL((bool)p["bool"], (bool)params["bool"]);
    BOOST_CHECK_EQUAL(p["int"], params["int"]);
    BOOST_CHECK_EQUAL(p["unsigned int"], params["unsigned int"]);
    BOOST_CHECK_EQUAL(p["long long"], params["long long"]);
    BOOST_CHECK_EQUAL(p["unsigned long long"], params["unsigned long long"]);
    BOOST_CHECK_EQUAL(p["double"], params["double"]);
    BOOST_CHECK_EQUAL(p["string"], params["string"]);
    BOOST_CHECK_EQUAL(p["Length"], params["Length"]);
    BOOST_CHECK(p["Date"].compare(params["Date"])); // FIXME: equality check fails
    BOOST_CHECK_EQUAL(p["PathName"], params["PathName"]);
}

BOOST_AUTO_TEST_CASE( test_composite_params )
{
    BOOST_TEST_MESSAGE("Initialize CompositeParams from ValueParams");

    Params cp(CompositeParams()
              .push_back(Params(ValueParams().set("bool", true)))
              .push_back(Params(ValueParams().set("int", imax)))
              .push_back(Params(ValueParams().set("unsigned int", uimax)))
              .push_back(Params(ValueParams().set("long long", llmax)))
              .push_back(Params(ValueParams().set("unsigned long long", ullmax)))
              .push_back(Params(ValueParams().set("double", dmax)))
              .push_back(Params(ValueParams().set("string", "foo")))
              .push_back(Params(ValueParams().set("Length", Length(42))))
              .push_back(Params(ValueParams().set("Date", Date(2015, 2, 1))))
              .push_back(Params(ValueParams().set("PathName", PathName("/var/tmp")))));

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
    BOOST_TEST_MESSAGE("Initialize CompositeParams from list of ValueParams");

    Params::List l;
    l.push_back(Params(ValueParams().set("bool", true)));
    l.push_back(Params(ValueParams().set("int", imax)));
    l.push_back(Params(ValueParams().set("unsigned int", uimax)));
    l.push_back(Params(ValueParams().set("long long", llmax)));
    l.push_back(Params(ValueParams().set("unsigned long long", ullmax)));
    l.push_back(Params(ValueParams().set("double", dmax)));
    l.push_back(Params(ValueParams().set("string", "foo")));
    l.push_back(Params(ValueParams().set("Length", Length(42))));
    l.push_back(Params(ValueParams().set("Date", Date(2015, 2, 1))));
    l.push_back(Params(ValueParams().set("PathName", PathName("/var/tmp"))));

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
    Params sp(ScopeParams("scope", Params(ValueParams().set("foo", "bar"))));
    BOOST_CHECK( sp.has("scope.foo") );
    BOOST_CHECK( !sp.has("foo") );
    BOOST_CHECK_EQUAL( sp["scope.foo"], "bar" );
    BOOST_CHECK_THROW( sp["foo"], BadParameter );
}

BOOST_AUTO_TEST_CASE( test_unscope_params )
{
    Params sp(UnScopeParams("scope", Params(ValueParams().set("scope.foo", "bar"))));
    BOOST_CHECK( !sp.has("scope.foo") );
    BOOST_CHECK( sp.has("foo") );
    BOOST_CHECK_THROW( sp["scope.foo"], BadParameter );
    BOOST_CHECK_EQUAL( sp["foo"], "bar" );
}

BOOST_AUTO_TEST_CASE( test_scope_unscope_params )
{
    Params user(ValueParams().set("resol", 100));
    Params def(ValueParams().set("resol", 200));

    Params p(CompositeParams()
             .push_back(Params(ScopeParams("user", user)))
             .push_back(Params(ScopeParams("default", def))));

    BOOST_CHECK( p.has("user.resol") );
    BOOST_CHECK( p.has("default.resol") );
    BOOST_CHECK( !p.has("resol") );
    BOOST_CHECK_EQUAL( (uint)p["user.resol"], 100 );
    BOOST_CHECK_EQUAL( (uint)p["default.resol"], 200 );

    Params usp(UnScopeParams("user", p));

    BOOST_CHECK( !usp.has("user.resol") );
    BOOST_CHECK( !usp.has("default.resol") );
    BOOST_CHECK( usp.has("resol") );
    BOOST_CHECK_EQUAL( (uint)usp["resol"], 100 );

    Params dsp(UnScopeParams("default", p));

    BOOST_CHECK( !dsp.has("user.resol") );
    BOOST_CHECK( !dsp.has("default.resol") );
    BOOST_CHECK( dsp.has("resol") );
    BOOST_CHECK_EQUAL( (uint)dsp["resol"], 200 );
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
