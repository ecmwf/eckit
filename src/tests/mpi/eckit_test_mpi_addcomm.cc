/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <mpi.h>

#include "eckit/mpi/Comm.h"

int main( int argc, char** argv )
{
    MPI_Init(&argc, &argv);

    MPI_Fint fortranComm = MPI_Comm_c2f(MPI_COMM_WORLD);

    eckit::mpi::addComm("fort.1", fortranComm);

    eckit::mpi::setCommDefault("fort.1");

    MPI_Finalize();

    return 0;
}
