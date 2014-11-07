/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/Call.h"
#include "eckit/xpr/Bind.h"

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Merge.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/Xpr.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// test lambda expressions

class TestLambda : public Tool {

public:

    TestLambda(int argc,char **argv): Tool(argc,argv)
    {
    }

    virtual void run();

    void setup();

    void test_lambda();

    void teardown();
};

void TestLambda::run()
{
    setup();

    test_lambda();

    teardown();
}

void TestLambda::setup()
{
}

void TestLambda::teardown()
{
}

void TestLambda::test_lambda()
{ 
    setformat(std::cout,xpr::CodeFormat);

    Xpr a = xpr::scalar( 2. );
    Xpr b = xpr::scalar( 4. );
    Xpr c = xpr::scalar( 1. );

    Xpr l = xpr::list(a, b, c);

    Xpr twice  = call(lambda("a", Xpr(2.0) * Xpr("a")));

//    std::cout << twice(c) << std::endl;

    ASSERT( Scalar::extract(twice(c)) == 2. );

    Xpr neg    = call(lambda("a", Xpr(0.0) - Xpr("a")));

//    std::cout << neg(a) << std::endl;

    ASSERT( Scalar::extract(neg(a)) == -2. );

    if( false ) // why does this work?
    {

    Xpr X = l + l;

    std::cout << X << std::endl;
    std::cout << X.expr()->str() << std::endl;


    X = X(); // evaluation to self

    std::cout << X << std::endl;
    std::cout << X.expr()->str() << std::endl;

    }

}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestLambda mytest(argc,argv);
    mytest.start();
    return 0;
}

