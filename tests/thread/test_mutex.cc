/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/Mutex.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

CASE( "TestMutex" )
{
    Mutex* m = 0;

    EXPECT_NO_THROW( m = new Mutex() );

    EXPECT_NO_THROW( m->lock() );

    EXPECT_NO_THROW( m->unlock() );

    delete m;
}


//-----------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main(int argc,char **argv)
{
    return run_tests ( argc, argv );
}

