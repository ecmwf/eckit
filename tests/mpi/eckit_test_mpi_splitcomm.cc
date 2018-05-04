/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include "eckit/mpi/Comm.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::mpi;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

int argc;
char ** argv;

CASE( "Test MPI splitComm" )
{
    Comm & all = eckit::mpi::comm();

    // Check splitting of communicator

    EXPECT( all.size() == 4 );
    EXPECT( all.rank() >= 0 );
    EXPECT( all.rank() < 4 );

    Comm * newcomm;
    if (all.rank() < 2) {
        newcomm = all.split(1, "FirstHalf");
    } else {
        newcomm = all.split(2, "SecondHalf");
    }

    EXPECT ( newcomm->size() == 2 );
    EXPECT ( newcomm->rank() >= 0 );
    EXPECT ( newcomm->rank() < 2 );
    EXPECT ( all.size() == 4 );

    // Check ordering of ranks is preserved

    switch( all.rank() ) {
        case 0:
            EXPECT( newcomm->rank() == 0 );
            break;
        case 1:
            EXPECT ( newcomm->rank() == 1 );
            break;
        case 2:
            EXPECT (newcomm->rank() == 0 );
            break;
        case 3:
            EXPECT (newcomm->rank() == 1 );
            break;
    }

    if (all.rank() < 2) {
        EXPECT(hasComm("FirstHalf"));
        EXPECT(!hasComm("SecondHalf"));
    } else {
        EXPECT(hasComm("SecondHalf"));
        EXPECT(!hasComm("FirstHalf"));
    }
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main(int argc,char **argv)
{
    eckit::test::argc = argc;
    eckit::test::argv = argv;
    return run_tests ( argc, argv );
}
