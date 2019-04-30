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

#include "eckit/mpi/Comm.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

int argc;
char** argv;

CASE("Test MPI addComm") {
    MPI_Init(&argc, &argv);

    MPI_Fint fortranComm = MPI_Comm_c2f(MPI_COMM_WORLD);

    eckit::mpi::addComm("fort.1", fortranComm);

    eckit::mpi::setCommDefault("fort.1");

    MPI_Finalize();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::test::argc = argc;
    eckit::test::argv = argv;
    return run_tests(argc, argv);
}
