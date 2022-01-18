/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/deprecated/LinearAlgebraCUDA.h"

#include <ostream>


namespace eckit {
namespace linalg {
namespace deprecated {


static const LinearAlgebraCUDA __la("cuda");


void eckit::linalg::deprecated::LinearAlgebraCUDA::print(std::ostream& out) const {
    out << "LinearAlgebraCUDA[]";
}


}  // namespace deprecated
}  // namespace linalg
}  // namespace eckit
