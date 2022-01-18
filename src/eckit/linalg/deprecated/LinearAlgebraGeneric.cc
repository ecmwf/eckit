/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/deprecated/LinearAlgebraGeneric.h"

#include <ostream>

#include "eckit/eckit.h"


namespace eckit {
namespace linalg {
namespace deprecated {


static const LinearAlgebraGeneric __la_generic("generic");

#ifdef eckit_HAVE_OMP
static const LinearAlgebraGeneric __la_openmp("openmp");
#endif

void LinearAlgebraGeneric::print(std::ostream& out) const {
    out << "LinearAlgebraGeneric[]";
}

// namespace


}  // namespace deprecated
}  // namespace linalg
}  // namespace eckit
