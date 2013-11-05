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
    using namespace maths;
#if 0
    ExpPtr a = scalar( 2. );
    ExpPtr c = scalar( 4. );

    VarPtr v1 = maths::vector( 10, 5. );
    VarPtr v2 = maths::vector( 10, 7. );

    ExpPtr e = add( prod( a, v1) , prod( c, v2 ));

    std::cout << *e << std::endl;
    std::cout << *e->signature() << std::endl;

    e = e->reduce();
#endif
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

