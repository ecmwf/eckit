// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/mpi/ParallelStatus.h"

#include <ostream>

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

void ParallelStatus::print(std::ostream& os) const {
    os << "ParallelStatus("
       << "source=" << source() << ",tag=" << tag() << ",error=" << error() << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
