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

#include "eckit/xpr/Xpr.h"
#include "eckit/xpr/Scalar.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Test if expression engine can curry parameters

class TestCurrying : public Tool {

public:

    TestCurrying(int argc,char **argv): Tool(argc,argv)
    {
    }

    virtual void run();

    void setup();

    void test_2args();
    void test_twice_1arg();
    void test_with_bind();
    void test_twice_lambda();

    void teardown();
};

void TestCurrying::run()
{
    setup();

    test_2args();
    test_twice_1arg();
    test_with_bind();
    test_twice_lambda();

    teardown();
}

void TestCurrying::setup()
{
}

void TestCurrying::teardown()
{
}

void TestCurrying::test_2args()
{
    Xpr X = lambda("i", "j", Xpr("i") + Xpr("j"));
    Xpr Y = call(X);

    Xpr r = Y(1.0, 2.4);

//    std::cout << "-----------------------" << std::endl;
//    std::cout << Y << std::endl;
//    std::cout << "-----------------------" << std::endl;
//    std::cout << r << std::endl;

    ASSERT( Scalar::extract(r) == 3.4 );
}

void TestCurrying::test_twice_1arg()
{
    Xpr X = lambda("i", call( lambda("j", Xpr("i") * Xpr("j"))));
    Xpr Y = call(X, Xpr( 2. ));

    Xpr r = Y( 3. );

//    std::cout << "-----------------------" << std::endl;
//    std::cout << Y << std::endl;
//    std::cout << "-----------------------" << std::endl;
//    std::cout << r << std::endl;

    ASSERT( Scalar::extract(r) == 6. );
}

void TestCurrying::test_with_bind()
{
    Xpr X = lambda("i", "j", Xpr("i") * Xpr("j"));
    Xpr Q = xpr::bind<2>(call(X), Xpr( 4. ));

    Xpr r = Q( 2. );

//    cout << r << endl;

    ASSERT( Scalar::extract(r) == 8. );
}

void TestCurrying::test_twice_lambda()
{
    Xpr C = call(
                  lambda("i",
                         call(
                             lambda("j",
                                      (Xpr("i") - Xpr("j")
                                       )
                                    )
                             )
                         ),
                 Xpr(2.0)); // bound to 'i'

    Xpr r = C( 4.0 ); // value for 'j'

//    std::cout << C << std::endl;
//    std::cout << r << std::endl;

    ASSERT( Scalar::extract(r) == -2. );
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestCurrying mytest(argc,argv);
    mytest.start();
    return 0;
}

