/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/ParallelStatus.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

void ParallelStatus::print(std::ostream& os) const {
    os << "ParallelStatus("
       << "source=" << source()
       << ",tag=" << tag()
       << ",error=" << error()
       << ")";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
