/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#define BOOST_TEST_MODULE test_eckit_resource

#include "ecbuild/boost_test_framework.h"

#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_resource )

BOOST_AUTO_TEST_CASE( test_default )
{
    string s = Resource<string>("s","some");

    BOOST_CHECK( s == "some" );

    double d = Resource<double>("d", 777.7);

    DEBUG_VAR(d);

    BOOST_CHECK( ( abs(d - 777.7) ) <= 10E-6 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_vector_long )
{
    std::vector<long> def(3,77);
    std::vector<long> v = Resource< std::vector<long> >("listlong;-listlong",def);

    BOOST_CHECK_EQUAL( v[0] , 88 );
    BOOST_CHECK_EQUAL( v[1] , 99 );
    BOOST_CHECK_EQUAL( v[2] , 11 );
    BOOST_CHECK_EQUAL( v[3] , 22 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_command_line )
{
    BOOST_CHECK( Resource<int>("integer;-integer",0) == 100 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_environment_var )
{
    char v [] = "TEST_ENV_INT=333";
    putenv(v);
    
    BOOST_CHECK( Resource<int>("intEnv;$TEST_ENV_INT",777) == 333 );

    char foo [] = "FOO=1Mb";
    putenv(foo);

    BOOST_CHECK( Resource<long>("$FOO",0) == 1024*1024);
    BOOST_CHECK( Resource<long>("$FOO;-foo",0) == 1024*1024);
    BOOST_CHECK( Resource<long>("-foo;$FOO",0) == 1024*1024);
    BOOST_CHECK( Resource<long>("$FOO;foo;-foo",0) == 1024*1024);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_config_file )
{
    ostringstream code;

    code << " buffer = 60 MB " << std::endl
         << " b = foo " << std::endl
         << " [ if class = od ] { b = bar }" << std::endl;

    istringstream in(code.str());

    ResourceMgr::instance().appendConfig(in);

    StringDict args;

    args["class"] = "od";

    string b = Resource<string>("b","none",args);

    DEBUG_VAR(b);

    BOOST_CHECK( b == "bar" );

    long buffer = Resource<long>("buffer",0);

    DEBUG_VAR(buffer);
    
    BOOST_CHECK( buffer == Bytes::MiB(60) );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

