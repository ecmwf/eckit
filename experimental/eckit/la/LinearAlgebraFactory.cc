/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/la/LinearAlgebraFactory.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

const LinearAlgebraBase* LinearAlgebraFactory::get() {
    return get(Resource<std::string>("-linearAlgebraBackend;linearAlgebraBackend", "generic"));
}

const LinearAlgebraBase* LinearAlgebraFactory::get(const std::string& name) {
    FactoryMap::const_iterator it = instance().map_.find(name);
    if (it == instance().map_.end())
        throw BadParameter("Linear algebra backend " + name + " not available.", Here());
    return it->second;
}

void LinearAlgebraFactory::regist(const std::string& name, const LinearAlgebraBase* backend) {
    instance().map_.insert(std::make_pair(name, backend));
}

LinearAlgebraFactory::LinearAlgebraFactory() {}

LinearAlgebraFactory &LinearAlgebraFactory::instance() {
    static LinearAlgebraFactory factory;
    return factory;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
