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

#include "eckit/grid/Grid.h"
#include "eckit/grid/Gaussian.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestGaussian : public Tool {
public:

    TestGaussian(int argc,char **argv): Tool(argc,argv) {}

    ~TestGaussian() {}

    virtual void run();

    void test_constructor();
};

//-----------------------------------------------------------------------------

void TestGaussian::test_constructor()
{
    using namespace eckit::grid;

    BoundBox2D earth ( Point2D(-90.,0.), Point2D(90.,360.) );
    Grid* g = NULL;

    // standard case

    g = new Gaussian( 48, earth );

    ASSERT( g );
    std::cout << "data size is " << g->dataSize() << std::endl;
    /// @todo review this: we wrap from 0 to 360 inclusive as we do for latlon
    ASSERT( g->dataSize() == (48 * 2 * ( 48 * 4 + 1) ) );

    /// @todo substitute these comparisons with proper floating point comparisons
    ASSERT( g->boundingBox().bottom_left_.lat_ == -90. );
    ASSERT( g->boundingBox().bottom_left_.lon_ ==   0. );
    ASSERT( g->boundingBox().top_right_.lat_ ==  90. );
    ASSERT( g->boundingBox().top_right_.lon_ == 360. );

}

//-----------------------------------------------------------------------------

void TestGaussian::run()
{
    test_constructor();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestGaussian mytest(argc,argv);
    mytest.start();
    return 0;
}

