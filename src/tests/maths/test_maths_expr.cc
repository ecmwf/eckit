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

#include "eckit/maths/Exp.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/BinaryFunc.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestExp : public Tool {
public:

    TestExp(int argc,char **argv): Tool(argc,argv) {}

    virtual void run();

    void test_add();

};

void TestExp::run()
{
    test_add();
}

void TestExp::test_add()
{
    using namespace eckit::maths;

    ExpPtr a = maths::scalar( 2. );
    ExpPtr b = maths::scalar( 4. );

    ExpPtr c = maths::add( a, b);

    ExpPtr x = maths::vector( 10, 5. );
    ExpPtr y = maths::vector( 10, 7. );

//    std::cout << *(maths::add( a ,a )->eval()) << std::endl;

    ExpPtr e = maths::add( prod( a, x) , prod( b, y ));

    std::cout << *e << std::endl;
    std::cout << e->signature() << std::endl;

//    e = e->reduce();

//    std::cout << *e << std::endl;
//    std::cout << e->signature() << std::endl;

    std::cout << *(e->eval()) << std::endl;

//    std::cout << *(maths::add(a,b)->reduce()) << std::endl;

//    std::cout << *(maths::add(a,b)->eval()) << std::endl;

//    std::cout << *(maths::add(a,x)->eval()) << std::endl;

}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestExp mytest(argc,argv);
    mytest.start();
    return 0;
}

