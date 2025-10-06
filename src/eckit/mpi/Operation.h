/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
