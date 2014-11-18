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

/// Test if expression engine can optimise expression trees without evaluating them

class TestOptimiser : public Tool {

    Xpr a;
    Xpr b;
    Xpr x;
    Xpr y;

public:

    TestOptimiser(int argc,char **argv): Tool(argc,argv),
      a( undef() ),
      b( undef() ),
      x( undef() ),
      y( undef() )
    {
    }

    virtual void run();

    void setup();

    void test_add_vv();
    void test_list_add_ss_vv();
    void test_linear_sv_sv();
    void test_linear_with_count();

    void teardown();
};

void TestOptimiser::run()
{
    setup();

    test_add_vv();
    test_list_add_ss_vv();
    test_linear_sv_sv();
    test_linear_with_count();

    teardown();
}

void TestOptimiser::setup()
{
    a = xpr::scalar( 2. );
    b = xpr::scalar( 4. );
    x = xpr::vector( 3, 5. );
    y = xpr::vector( 3, 7. );
}

void TestOptimiser::teardown()
{
}

void TestOptimiser::test_add_vv()
{
    // should not optimise

    Xpr X = xpr::add(x,y);

    ASSERT( X.optimise().expr()->str() == "Add(Vector(5, 5, 5), Vector(7, 7, 7))" );
}

void TestOptimiser::test_list_add_ss_vv()
{
    Xpr X = xpr::list( xpr::add(a,b), xpr::add(x,y));

    ASSERT( X.optimise().expr()->str() == "List(Scalar(6), Add(Vector(5, 5, 5), Vector(7, 7, 7)))" );
}

void TestOptimiser::test_linear_sv_sv()
{
    Xpr e = a+b;
    Xpr z = (a-b) * e;
    Xpr X = xpr::add( xpr::prod(e,x), xpr::prod(z,y));

    ASSERT( X.optimise().expr()->str() == "Linear(Scalar(6), Vector(5, 5, 5), Scalar(-12), Vector(7, 7, 7))" );
}

void TestOptimiser::test_linear_with_count()
{
    Xpr e = xpr::count(
                xpr::merge(
                    xpr::list(a,a,a,a,a,a,a),
                           xpr::list(b,b,b))
                             );
    Xpr X = xpr::add( xpr::prod(e,x), xpr::prod(b,y));

    ASSERT( X.optimise().expr()->str() == "Linear(Scalar(10), Vector(5, 5, 5), Scalar(4), Vector(7, 7, 7))" );
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestOptimiser mytest(argc,argv);
    mytest.start();
    return 0;
}

