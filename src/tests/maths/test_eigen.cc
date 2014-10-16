/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Eigen.h"
#include "eckit/runtime/Tool.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestEigen : public Tool {
public:

    TestEigen(int argc,char **argv): Tool(argc,argv) {}

    virtual void run();

    void setup();

    void test_vector();

    void teardown();
};

//-----------------------------------------------------------------------------

void TestEigen::setup()
{
}

//-----------------------------------------------------------------------------

void TestEigen::teardown()
{
}

//-----------------------------------------------------------------------------

void TestEigen::test_vector()
{
	Eigen::Vector3d v1;
	Eigen::Vector3d v2;
	Eigen::Vector3d r;

	r = v2 - v1;
}

//-----------------------------------------------------------------------------

void TestEigen::run()
{
    setup();

    test_vector();

    teardown();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestEigen mytest(argc,argv);
    mytest.start();
    return 0;
}

