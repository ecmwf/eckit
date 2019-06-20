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

//----------------------------------------------------------------------------------------------------------------------

int argc;
char** argv;

CASE("Test MPI Communicator Split") {
    // Get default communicator
    Comm& all = eckit::mpi::comm();

    std::string start = all.name();

    // Check sizes and ranks
    EXPECT(all.size() == 4);
    EXPECT(all.rank() < 4);

    // Split communicator in half
    if (all.rank() < 2) {
        all.split(1, "FirstHalf");
    }
    else {
        all.split(2, "SecondHalf");
    }

    // Fetch the new communicator
    const Comm& newcomm = eckit::mpi::comm((all.rank() < 2 ? "FirstHalf" : "SecondHalf"));

    // Check sizes and ranks
    EXPECT(newcomm.size() == 2);
    EXPECT(newcomm.rank() < 2);
    EXPECT(all.size() == 4);

    // Check rank order is preserved
    switch (all.rank()) {
        case 0:
            EXPECT(newcomm.rank() == 0);
            break;
        case 1:
            EXPECT(newcomm.rank() == 1);
            break;
        case 2:
            EXPECT(newcomm.rank() == 0);
            break;
        case 3:
            EXPECT(newcomm.rank() == 1);
            break;
    }

    // Check naming of communicator in environment
    if (all.rank() < 2) {
        EXPECT(hasComm("FirstHalf"));
        EXPECT(!hasComm("SecondHalf"));
    }
    else {
        EXPECT(hasComm("SecondHalf"));
        EXPECT(!hasComm("FirstHalf"));
    }

    // Also check setting of default communicator (typical use-case)
    if (all.rank() < 2) {
        setCommDefault("FirstHalf");
    }
    else {
        setCommDefault("SecondHalf");
    }

    Comm& new_default = eckit::mpi::comm();
    EXPECT(new_default.size() == 2);


    // test freeing up the default communicator
    if (all.rank() < 2) {
        EXPECT_THROWS_AS(deleteComm("FirstHalf"), eckit::SeriousBug);
    }
    else {
        EXPECT_THROWS_AS(deleteComm("SecondHalf"), eckit::SeriousBug);
    }

    EXPECT_THROWS_AS(deleteComm("Foo"), eckit::SeriousBug);

    setCommDefault(start.c_str());

    // test freeing up the default communicator
    if (all.rank() < 2) {
        EXPECT_NO_THROW(deleteComm("FirstHalf"));
        EXPECT(not eckit::mpi::hasComm("FirstHalf"));
    }
    else {
        EXPECT_NO_THROW(deleteComm("SecondHalf"));
        EXPECT(not eckit::mpi::hasComm("SecondHalf"));
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::test::argc = argc;
    eckit::test::argv = argv;
    return run_tests(argc, argv);
}
