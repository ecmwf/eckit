/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_value_params_streaming

#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/FileStream.h"
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

    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << p;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params params(Params::decode(sin));
        BOOST_TEST_MESSAGE("original: " << p);
        BOOST_TEST_MESSAGE("streamed: " << params);
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

    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << p;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params params(Params::decode(sin));
        BOOST_TEST_MESSAGE("original: " << p);
        BOOST_TEST_MESSAGE("streamed: " << params);
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

    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << cp;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params params(Params::decode(sin));
        BOOST_TEST_MESSAGE("original: " << cp);
        BOOST_TEST_MESSAGE("streamed: " << params);
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

    Params pin( (CompositeParams(l)) ); // C++ Most Vexing Parse
    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << pin;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params params(Params::decode(sin));
        BOOST_TEST_MESSAGE("original: " << pin);
        BOOST_TEST_MESSAGE("streamed: " << params);
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
}

BOOST_AUTO_TEST_CASE( test_scope_params )
{
    Params pin(ScopeParams("scope", Params(Properties().set("foo", "bar"))));
    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << pin;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params sp(Params::decode(sin));
        BOOST_TEST_MESSAGE("original: " << pin);
        BOOST_TEST_MESSAGE("streamed: " << sp);
        BOOST_CHECK( sp.has("scope.foo") );
        BOOST_CHECK( !sp.has("foo") );
        BOOST_CHECK_EQUAL( sp["scope.foo"], "bar" );
        BOOST_CHECK_THROW( sp["foo"], BadParameter );
    }
}

BOOST_AUTO_TEST_CASE( test_composite_scope_params )
{
    Params user(Properties().set("resol", 100));
    Params def(Properties().set("resol", 200));

    Params pin(CompositeParams()
               .push_back(Params(ScopeParams("user", user)))
               .push_back(Params(ScopeParams("default", def))));

    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << pin;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params p(Params::decode(sin));
        BOOST_CHECK( p.has("user.resol") );
        BOOST_CHECK( p.has("default.resol") );
        BOOST_CHECK( !p.has("resol") );
        BOOST_CHECK_EQUAL( (uint)p["user.resol"], 100 );
        BOOST_CHECK_EQUAL( (uint)p["default.resol"], 200 );
    }
}

BOOST_AUTO_TEST_CASE( test_dispatch_params )
{
    Params pin( (TestParams("bar")) ); // C++ Most Vexing Parse
    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << pin;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params p(Params::decode(sin));
        BOOST_CHECK( p.has("foo") );
        BOOST_CHECK( !p.has("bar") );
        BOOST_CHECK_EQUAL( p["foo"], "bar" );
    }
}

BOOST_AUTO_TEST_CASE( test_custom_params )
{
    Params pin( (AnyKeyParams("foo")) ); // C++ Most Vexing Parse
    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << pin;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Params p(Params::decode(sin));
        BOOST_CHECK( p.has("foo") );
        BOOST_CHECK( p.has("bar") );
        BOOST_CHECK_EQUAL( p["foo"], "foo" );
        BOOST_CHECK_EQUAL( p["bar"], "foo" );
    }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
