/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "experimental/eckit/la/LinearAlgebra.h"
#include "eckit/log/Log.h"

namespace eckit {
namespace la {

namespace {

typedef std::map<std::string, const LinearAlgebra*> BackendMap;
static BackendMap m;

static std::string currentBackend = "generic";

}  // anonymous namespace

//-----------------------------------------------------------------------------

const LinearAlgebra* LinearAlgebra::backend() {
    BackendMap::const_iterator it = m.find(currentBackend);
    if (it == m.end())
        throw BadParameter("Linear algebra backend " + currentBackend + " not available.", Here());
    Log::debug() << "Using LinearAlgebra backend " << it->first << std::endl;
    return it->second;
}

void LinearAlgebra::backend(const std::string& name) {
    BackendMap::const_iterator it = m.find(name);
    if (it == m.end())
        throw BadParameter("Linear algebra backend " + name + " not available.", Here());
    Log::info() << "Setting LinearAlgebra backend to " << name << std::endl;
    currentBackend = name;
}

void LinearAlgebra::list(std::ostream& out)
{
    const char* sep = "";
    for (BackendMap::const_iterator it = m.begin() ; it != m.end() ; ++it) {
        out << sep << it->first;
        sep = ", ";
    }
}

LinearAlgebra::LinearAlgebra(const std::string& name) {
    ASSERT(m.find(name) == m.end());
    m[name] = this;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
