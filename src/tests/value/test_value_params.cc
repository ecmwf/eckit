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

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_value_params )

BOOST_AUTO_TEST_CASE( test_value_params_keys )
{
    BOOST_TEST_MESSAGE("Test keys present in ValueParams");
    ValueParams p;
    p.set("bool", true);
    p.set("int", numeric_limits<int>::max());
    p.set("unsigned int", numeric_limits<unsigned int>::max());
    p.set("long long", numeric_limits<long long>::max());
    p.set("unsigned long long", numeric_limits<unsigned long long>::max());
    p.set("double", numeric_limits<double>::max());
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
}

BOOST_AUTO_TEST_CASE( test_value_params )
{
    BOOST_TEST_MESSAGE("Initialize ValueParams");
    ValueParams p;
    p.set("bool", true);
    p.set("int", numeric_limits<int>::max());
    p.set("unsigned int", numeric_limits<unsigned int>::max());
    p.set("long long", numeric_limits<long long>::max());
    p.set("unsigned long long", numeric_limits<unsigned long long>::max());
    p.set("double", numeric_limits<double>::max());
    p.set("string", "foo");
    p.set("Length", Length(42));
    p.set("Date", Date(2015, 2, 1));
    p.set("PathName", PathName("/var/tmp"));

    Params params(p);
    BOOST_TEST_MESSAGE("Params: " << params);
    BOOST_CHECK_EQUAL(true, (bool)params["bool"]);
    BOOST_CHECK_EQUAL(numeric_limits<int>::max(), (int)params["int"]);
    BOOST_CHECK_EQUAL(numeric_limits<unsigned int>::max(),
                      (unsigned int)params["unsigned int"]);
    BOOST_CHECK_EQUAL(numeric_limits<long long>::max(),
                      (long long)params["long long"]);
    BOOST_CHECK_EQUAL(numeric_limits<unsigned long long>::max(),
                      (unsigned long long)params["unsigned long long"]);
    BOOST_CHECK_EQUAL(numeric_limits<double>::max(), (double)params["double"]);
    BOOST_CHECK_EQUAL("foo", params["string"]);
    BOOST_CHECK_EQUAL(Length(42), params["Length"]);
    BOOST_CHECK(params["Date"].compare(Date(2015, 2, 1))); // FIXME: equality check fails
    BOOST_CHECK_EQUAL(PathName("/var/tmp"), params["PathName"]);
}

BOOST_AUTO_TEST_CASE( test_value_params_from_properties )
{
    BOOST_TEST_MESSAGE("Initialize ValueParams from Properties");
    Properties p;
    p.set("bool", true);
    p.set("int", numeric_limits<int>::max());
    p.set("unsigned int", numeric_limits<unsigned int>::max());
    p.set("long long", numeric_limits<long long>::max());
    p.set("unsigned long long", numeric_limits<unsigned long long>::max());
    p.set("double", numeric_limits<double>::max());
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

    CompositeParams cp;
    cp.push_back(Params(ValueParams().set("bool", true)));
    cp.push_back(Params(ValueParams().set("int", numeric_limits<int>::max())));
    cp.push_back(Params(ValueParams().set("unsigned int", numeric_limits<unsigned int>::max())));
    cp.push_back(Params(ValueParams().set("long long", numeric_limits<long long>::max())));
    cp.push_back(Params(ValueParams().set("unsigned long long", numeric_limits<unsigned long long>::max())));
    cp.push_back(Params(ValueParams().set("double", numeric_limits<double>::max())));
    cp.push_back(Params(ValueParams().set("string", "foo")));
    cp.push_back(Params(ValueParams().set("Length", Length(42))));
    cp.push_back(Params(ValueParams().set("Date", Date(2015, 2, 1))));
    cp.push_back(Params(ValueParams().set("PathName", PathName("/var/tmp"))));

    Params params(cp);
    BOOST_TEST_MESSAGE("Params: " << params);
    BOOST_CHECK_EQUAL(true, (bool)params["bool"]);
    BOOST_CHECK_EQUAL(numeric_limits<int>::max(), (int)params["int"]);
    BOOST_CHECK_EQUAL(numeric_limits<unsigned int>::max(),
                      (unsigned int)params["unsigned int"]);
    BOOST_CHECK_EQUAL(numeric_limits<long long>::max(),
                      (long long)params["long long"]);
    BOOST_CHECK_EQUAL(numeric_limits<unsigned long long>::max(),
                      (unsigned long long)params["unsigned long long"]);
    BOOST_CHECK_EQUAL(numeric_limits<double>::max(), (double)params["double"]);
    BOOST_CHECK_EQUAL("foo", params["string"]);
    BOOST_CHECK_EQUAL(Length(42), params["Length"]);
    BOOST_CHECK(params["Date"].compare(Date(2015, 2, 1))); // FIXME: equality check fails
    BOOST_CHECK_EQUAL(PathName("/var/tmp"), params["PathName"]);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
