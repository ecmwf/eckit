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

/// Test if expression engine can optimise expression trees without evaluating them

class TestOptimiser : public Tool {

    Math a;
    Math b;
    Math x;
    Math y;

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
    a = maths::scalar( 2. );
    b = maths::scalar( 4. );
    x = maths::vector( 3, 5. );
    y = maths::vector( 3, 7. );
}

void TestOptimiser::teardown()
{
}

void TestOptimiser::test_add_vv()
{
    // should not optimise

    Math X = maths::add(x,y);

    ASSERT( X.optimise().expr()->str() == "Add(Vector(5, 5, 5), Vector(7, 7, 7))" );
}

void TestOptimiser::test_list_add_ss_vv()
{
    Math X = maths::list( maths::add(a,b), maths::add(x,y));

    ASSERT( X.optimise().expr()->str() == "List(Scalar(6), Add(Vector(5, 5, 5), Vector(7, 7, 7)))" );
}

void TestOptimiser::test_linear_sv_sv()
{
    Math e = a+b;
    Math z = (a-b) * e;
    Math X = maths::add( maths::prod(e,x), maths::prod(z,y));

    ASSERT( X.optimise().expr()->str() == "Linear(Scalar(6), Vector(5, 5, 5), Scalar(-12), Vector(7, 7, 7))" );
}

void TestOptimiser::test_linear_with_count()
{
    Math e = maths::count(
                maths::merge(
                    maths::list(a,a,a,a,a,a,a),
                           maths::list(b,b,b))
                             );
    Math X = maths::add( maths::prod(e,x), maths::prod(b,y));

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

