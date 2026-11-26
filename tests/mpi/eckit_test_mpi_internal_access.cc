// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include "eckit/mpi/Parallel.h"
#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Test internal access to MPI_Comm") {

    auto* comm = dynamic_cast<eckit::mpi::Parallel*>(&eckit::mpi::comm());
    EXPECT(comm != nullptr);  // Successful dynamic cast

    MPI_Comm mpi_comm = comm->MPIComm();

    // Check size
    {
        int size;
        EXPECT(MPI_Comm_size(mpi_comm, &size) == MPI_SUCCESS);
        EXPECT(size == eckit::mpi::comm().size());
    }

    // Check rank
    {
        int rank;
        EXPECT(MPI_Comm_rank(mpi_comm, &rank) == MPI_SUCCESS);
        EXPECT(rank == eckit::mpi::comm().rank());
    }
}

//----------------------------------------------------------------------------------------------------------------------

// nc_create_par from the NetCDF library is an example routine with following signature that requires
// access to internal MPI_Comm and MPI_INFO_NULL.
int mock_nc_create_par(const char* path, int cmode, MPI_Comm comm, MPI_Info info, int* ncidp) {
    EXPECT(MPI_Barrier(comm) == MPI_SUCCESS);
    return 0;
}

CASE("Test internal access to MPI_Comm for mock NetCDF routine") {
    auto* comm = dynamic_cast<eckit::mpi::Parallel*>(&eckit::mpi::comm());
    EXPECT(comm != nullptr);  // Successful dynamic cast

    MPI_Comm mpi_comm = comm->MPIComm();
    MPI_Info mpi_info = MPI_INFO_NULL;

    int* ncidp = nullptr;
    int cmode  = 0;
    mock_nc_create_par("mock_file.nc", cmode, mpi_comm, mpi_info, ncidp);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
