// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/mpi/Operation.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

Operation::Code sum() {
    return Operation::SUM;
}

Operation::Code prod() {
    return Operation::PROD;
}

Operation::Code max() {
    return Operation::MAX;
}

Operation::Code min() {
    return Operation::MIN;
}

Operation::Code maxloc() {
    return Operation::MAXLOC;
}

Operation::Code minloc() {
    return Operation::MINLOC;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi
