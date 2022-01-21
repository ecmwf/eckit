/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/LinearAlgebra.h"

#include <set>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/StringTools.h"

namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


///@note __backend supports the deprecated functionality, to remove
static const struct LinearAlgebraInstance : public LinearAlgebra {
    LinearAlgebraInstance() = default;
} __backend;


//-----------------------------------------------------------------------------


const LinearAlgebra& LinearAlgebra::backend() {
    return __backend;
}


void LinearAlgebra::backend(const std::string& name) {
    ASSERT(LinearAlgebraDense::hasBackend(name) || LinearAlgebraSparse::hasBackend(name));

    if (LinearAlgebraDense::hasBackend(name)) {
        LinearAlgebraDense::backend(name);
    }

    if (LinearAlgebraSparse::hasBackend(name)) {
        LinearAlgebraSparse::backend(name);
    }
}


std::ostream& LinearAlgebra::list(std::ostream& out) {
    const auto* delimiter = ", ";
    const auto* sep       = "";

    std::ostringstream str;
    LinearAlgebraDense::list(str);
    LinearAlgebraSparse::list(str.str().empty() ? str : (str << delimiter));

    for (const auto& backend : [](const std::vector<std::string>& unsorted) {
             return std::set<std::string>(unsorted.begin(), unsorted.end());
         }(StringTools::split(delimiter, str.str()))) {
        out << sep << backend;
        sep = delimiter;
    }

    return out;
}


const LinearAlgebra& LinearAlgebra::getBackend(const std::string& name) {
    backend(name);
    return __backend;
}


bool LinearAlgebra::hasBackend(const std::string& name) {
    return LinearAlgebraDense::hasBackend(name) || LinearAlgebraSparse::hasBackend(name);
}


const LinearAlgebraDense& LinearAlgebra::getDenseBackend(const std::string& name) {
    return LinearAlgebraDense::getBackend(name);
}


const LinearAlgebraSparse& LinearAlgebra::getSparseBackend(const std::string& name) {
    return LinearAlgebraSparse::getBackend(name);
}


const LinearAlgebraDense& LinearAlgebra::denseBackend(const std::string& name) {
    return LinearAlgebraDense::backend(name);
}


const LinearAlgebraSparse& LinearAlgebra::sparseBackend(const std::string& name) {
    return LinearAlgebraSparse::backend(name);
}


bool LinearAlgebra::hasDenseBackend(const std::string& name) {
    return LinearAlgebraDense::hasBackend(name);
}


bool LinearAlgebra::hasSparseBackend(const std::string& name) {
    return LinearAlgebraSparse::hasBackend(name);
}


std::string LinearAlgebra::name() {
    const auto dense  = LinearAlgebraDense::name();
    const auto sparse = LinearAlgebraSparse::name();
    return dense == sparse ? dense : (dense + "/" + sparse);
}


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
