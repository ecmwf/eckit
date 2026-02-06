/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include <mpi.h>
#include <cstdlib>

#include "eckit/mpi/Comm.h"
#include "eckit/mpi/Parallel.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

int argc;
char** argv;

CASE("Setup MPI environment") {
    MPI_Init(&argc, &argv);
    eckit::mpi::comm();  // Force MPI environment to be setup with defaults
}

CASE("Test MPI addComm") {
    MPI_Fint fortranComm = MPI_Comm_c2f(MPI_COMM_WORLD);

    eckit::mpi::addComm("fort.1", fortranComm);
    EXPECT(eckit::mpi::hasComm("fort.1"));

    MPI_Comm external_comm = MPI_COMM_SELF;
    eckit::mpi::addComm("external_comm", MPI_Comm_c2f(external_comm));
    EXPECT(eckit::mpi::hasComm("external_comm"));
}

CASE("Test MPI unregisterComm") {
    EXPECT_THROWS_AS(eckit::mpi::unregisterComm("world"), eckit::SeriousBug);  // Cannot unregister 'world' communicator
    EXPECT_THROWS_AS(eckit::mpi::unregisterComm("self"), eckit::SeriousBug);   // Cannot unregister 'self' communicator

    EXPECT(eckit::mpi::hasComm("fort.1"));
    eckit::mpi::setCommDefault("fort.1");

    EXPECT_THROWS_AS(eckit::mpi::unregisterComm("fort.1"),
                     eckit::SeriousBug);  // Cannot unregister default communicator
    eckit::mpi::setCommDefault("world");

    // Following two exceptions are only thrown with the paralle mpi-backend, hence the setenv("ECKIT_MPI_FORCE",
    // "parallel", 1) in main()
    EXPECT_THROWS_AS(eckit::mpi::deleteComm("fort.1"),
                     eckit::SeriousBug);  // Cannot delete this communicator as it aliases MPI_COMM_WORLD
    EXPECT_THROWS_AS(eckit::mpi::deleteComm("external_comm"),
                     eckit::SeriousBug);  // Cannot delete this communicator as it aliases MPI_COMM_SELF

    EXPECT_NO_THROW(eckit::mpi::unregisterComm("fort.1"));
    EXPECT_NO_THROW(eckit::mpi::unregisterComm("external_comm"));
    EXPECT_NOT(eckit::mpi::hasComm("fort.1"));
    EXPECT_NOT(eckit::mpi::hasComm("external_comm"));
}

CASE("Teardown MPI environment") {
    MPI_Finalize();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    ::setenv("ECKIT_MPI_FORCE", "parallel", 1);  // Force parallel mpi-backend for this test
    eckit::test::argc = argc;
    eckit::test::argv = argv;
    return run_tests(argc, argv);
}
