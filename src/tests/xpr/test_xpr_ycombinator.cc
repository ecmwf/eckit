/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/xpr/Call.h"
#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Take.h"
#include "eckit/xpr/Xpr.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Test if expression engine can define a y-combinator

class TestYCombinator : public Tool {

	Xpr Y; ///< y-combinator math expression

public:

    TestYCombinator(int argc,char **argv): Tool(argc,argv), Y( undef() )
    {
    }

    virtual void run();

    void setup();

    /// test y-combinator implmentation of factorial function
    void test_ycombinator_factorial();
    /// test y-combinator implmentation of fibonacci function
    void test_ycombinator_fibonacci();

    void teardown();
};

void TestYCombinator::run()
{
    setup();

    test_ycombinator_factorial();
    test_ycombinator_fibonacci();

    teardown();
}

void TestYCombinator::setup()
{
	Y = lambda("f", call(Xpr("f"), Xpr("f")));
}

void TestYCombinator::teardown()
{
}

void TestYCombinator::test_ycombinator_factorial()
{
    // define the factorial (non-recursive) lambda function

	Xpr fact = lambda("f",
                       call(lambda("n",
                                   ifelse(
									   Xpr("n") == Xpr(0.0),
									   Xpr(1.0),
									   Xpr("n") * call( Xpr("f"), Xpr("f"), Xpr("n") - Xpr(1.0))
                                       )
                                   )
                            )
                       );


    // use y-combinator to recurse the function

	Xpr factorial = call(Y, fact);

//    setformat(cout, xpr::CodeFormat);
//    std::cout << factorial << std::endl;
//    setformat(cout, 0);

    // compute some values

	Xpr fac3 = factorial( 3. );
	Xpr fac5 = factorial( 5. );
	Xpr fac8 = factorial( 8. );

    // check results values

    ASSERT( Scalar::extract(fac3) == 6 );
    ASSERT( Scalar::extract(fac5) == 120 );
    ASSERT( Scalar::extract(fac8) == 40320 );

    //    std::cout << fac3 << std::endl;
    //    std::cout << fac5 << std::endl;
    //    std::cout << fac8 << std::endl;
}

void TestYCombinator::test_ycombinator_fibonacci()
{
    // define the fibonacci lookup-table to speed-up the lower order evaluations

	Xpr fibtable = xpr::list( Xpr(0.), Xpr(1.), Xpr(1.), Xpr(2.), Xpr(3.), Xpr(5.),
								 Xpr(8.), Xpr(13.), Xpr(21.), Xpr(34.), Xpr(55.) );

    // define the factorial (non-recursive) lambda function

	Xpr fib = lambda("f",
                      call(lambda("n",
                                  ifelse(
									  Xpr("n") > Xpr(10.),
									  Xpr(call(Xpr("f"), Xpr("f"), Xpr("n") - Xpr(1.0))) +
									  Xpr(call(Xpr("f"), Xpr("f"), Xpr("n") - Xpr(2.0))),
									  take( Xpr("n"), fibtable )
                                      )
                                  )
                           )
                      );

    // use y-combinator to recurse the function

	Xpr fibonnaci = call(Y, fib);

    // compute some fibonacci numbers

	Xpr f5  = fibonnaci( 5. );
	Xpr f10 = fibonnaci( 10.);
	Xpr f20 = fibonnaci( 20.);
//    Math f30 = fibonnaci( 30.); // -- rather slow test

    // check results values

    ASSERT( Scalar::extract(f5)  == 5 );
    ASSERT( Scalar::extract(f10) == 55 );
    ASSERT( Scalar::extract(f20) == 6765 );
//    ASSERT( Scalar::extract(f30) == 832040 ); // -- rather slow test

//    std::cout << f5  << std::endl;
//    std::cout << f10 << std::endl;
//    std::cout << f20 << std::endl;
//    std::cout << f30 << std::endl;
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestYCombinator mytest(argc,argv);
    mytest.start();
    return 0;
}

