// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_Operation_h
#define eckit_mpi_Operation_h

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

struct Operation {

    enum Code {
        SUM = 0,
        PROD,
        MAX,
        MIN,
        MAXLOC,
        MINLOC,
        MAX_OPERATION_CODE,
    };
};

Operation::Code sum();
Operation::Code prod();
Operation::Code max();
Operation::Code min();
Operation::Code maxloc();
Operation::Code minloc();

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
