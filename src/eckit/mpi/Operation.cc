/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Operation.h"

namespace eckit {
namespace mpi {

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

}  // namespace mpi
}  // namespace eckit
