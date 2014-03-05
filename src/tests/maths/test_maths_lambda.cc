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

#include "eckit/maths/IfElse.h"
#include "eckit/maths/Lambda.h"
#include "eckit/maths/ParamDef.h"
#include "eckit/maths/Call.h"
#include "eckit/maths/Bind.h"

#include "eckit/maths/Math.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/List.h"
#include "eckit/maths/BinaryOperator.h"
#include "eckit/maths/Count.h"
#include "eckit/maths/Merge.h"

using namespace std;
using namespace eckit;
using namespace eckit::maths;

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
    setformat(std::cout,maths::CodeFormat);

    Math a = maths::scalar( 2. );
    Math b = maths::scalar( 4. );
    Math c = maths::scalar( 1. );

    Math l = maths::list(a, b, c);

    Math twice  = call(lambda("a", Math(2.0) * Math("a")));

//    std::cout << twice(c) << std::endl;

    ASSERT( Scalar::extract(twice(c)) == 2. );

    Math neg    = call(lambda("a", Math(0.0) - Math("a")));

//    std::cout << neg(a) << std::endl;

    ASSERT( Scalar::extract(neg(a)) == -2. );

    if( false ) // why does this work?
    {

    Math X = l + l;

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

