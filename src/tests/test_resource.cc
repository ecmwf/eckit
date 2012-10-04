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

#include "eclib/Log.h"
#include "eclib/Tool.h"
#include "eclib/Resource.h"
#include "eclib/Types.h"

class TestResource : public Tool {
public:

    TestResource(int argc,char **argv): Tool(argc,argv) {}

    ~TestResource() {}

    virtual void run();

    void test_default();
    void test_command_line();
    void test_environment_var();
    void test_config_file();

};

//-----------------------------------------------------------------------------

void TestResource::test_default()
{
    string s = Resource<string>("s","some");

    ASSERT( s == "some" );

    double d = Resource<double>("d", 777.7);

    ASSERT( ( abs(d - 777.7) ) <= 10E-6 );
}

//-----------------------------------------------------------------------------

void TestResource::test_command_line()
{
    int integer = Resource<int>("integer;-integer",0);

    ASSERT( integer == 100 );
}

//-----------------------------------------------------------------------------

void TestResource::test_environment_var()
{
    int intEnv = Resource<int>("intEnv;$TEST_ENV_INT",777);

    ASSERT( intEnv == 333 );

    putenv("FOO=1Mb");

    ASSERT(Resource<long>("$FOO",0) == 1024*1024);
    ASSERT(Resource<long>("$FOO;-foo",0) == 1024*1024);
    ASSERT(Resource<long>("-foo;$FOO",0) == 1024*1024);
    ASSERT(Resource<long>("$FOO;foo;-foo",0) == 1024*1024);

}

//-----------------------------------------------------------------------------

void TestResource::test_config_file()
{
}

//-----------------------------------------------------------------------------
            
void TestResource::run()
{
    test_default();
    test_command_line();
    test_environment_var();
    test_config_file();
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestResource mytest(argc,argv);
    mytest.start();
    return 0;
}

