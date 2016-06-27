/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

struct TestFixture : public eckit::Tool {
public:

	TestFixture() : Tool( boost::unit_test::framework::master_test_suite().argc,
						  boost::unit_test::framework::master_test_suite().argv )
	{
//		for( int i = 0; i <  boost::unit_test::framework::master_test_suite().argc; ++i )
//			std::cout << "[" << boost::unit_test::framework::master_test_suite().argv[i] << "]" << std::endl;
	}

	virtual void run() {}
};

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_GLOBAL_FIXTURE( TestFixture );

BOOST_AUTO_TEST_SUITE( test_eckit_resource )

BOOST_AUTO_TEST_CASE( test_default )
{
    string s = Resource<string>("s","some");

    BOOST_CHECK( s == "some" );

    double d = Resource<double>("d", 777.7);

	BOOST_CHECK_CLOSE( d , 777.7, 0.0001 ); // accept 0.0001% tolerance
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
	int myint = Resource<int>("integer;-integer",0);
	BOOST_CHECK_EQUAL( myint , 100 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_environment_var )
{
    char v [] = "TEST_ENV_INT=333";
    putenv(v);
    
	int intenv = Resource<int>("intEnv;$TEST_ENV_INT",777);
	BOOST_CHECK_EQUAL( intenv , 333 );

    char foo [] = "FOO=1Mb";
    putenv(foo);

	long l1 = Resource<long>("$FOO",0);
	BOOST_CHECK_EQUAL( l1 , 1024*1024 );

	long l2 = Resource<long>("$FOO;-foo",0);
	BOOST_CHECK_EQUAL( l2 , 1024*1024);

	long l3 = Resource<long>("-foo;$FOO",0);
	BOOST_CHECK_EQUAL( l3, 1024*1024);

	long l4 = Resource<long>("$FOO;foo;-foo",0);
	BOOST_CHECK_EQUAL( l4 , 1024*1024);
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

	std::string bar = Resource<string>("b","none",args);

	BOOST_CHECK_EQUAL( bar , std::string("bar") );

    unsigned long long buffer = Resource<unsigned long long>("buffer",0);
    
	BOOST_CHECK_EQUAL( buffer , Bytes::MiB(60) );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

