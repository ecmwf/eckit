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
#include "experimental/eckit/la/LinearAlgebraFactory.h"
#include "eckit/log/Log.h"

namespace eckit {
namespace la {

namespace {
  static std::string currentBackend = "generic";
}  // anonymous namespace

//-----------------------------------------------------------------------------

const LinearAlgebraBase* LinearAlgebraFactory::get() {
    return get(currentBackend);
}

const LinearAlgebraBase* LinearAlgebraFactory::get(const std::string& name) {
    FactoryMap::const_iterator it = instance().map_.find(name);
    if (it == instance().map_.end())
        throw BadParameter("Linear algebra backend " + name + " not available.", Here());
    Log::debug() << "LinearAlgebraFactory: using backend " << it->first << std::endl;
    return it->second;
}

void LinearAlgebraFactory::set(const std::string& name) {
    currentBackend = name;
}

void LinearAlgebraFactory::regist(const std::string& name, const LinearAlgebraBase* backend) {
    instance().map_.insert(std::make_pair(name, backend));
}

void LinearAlgebraFactory::list(std::ostream& out)
{
    const char* sep = "";
    for (FactoryMap::const_iterator it = instance().map_.begin() ; it != instance().map_.end() ; ++it) {
        out << sep << it->first;
        sep = ", ";
    }
}

LinearAlgebraFactory::LinearAlgebraFactory() {}

LinearAlgebraFactory &LinearAlgebraFactory::instance() {
    static LinearAlgebraFactory factory;
    static LinearAlgebraFactory::Config config;
    return factory;
}

//-----------------------------------------------------------------------------

LinearAlgebraFactory::Config::Config()
    : backend_(this, "-linearAlgebraBackend;linearAlgebraBackend", "generic") {}

void LinearAlgebraFactory::Config::reconfigure() {
    currentBackend = backend_.value();
    Log::info() << "Reconfiguring linear algebra backend to: " << currentBackend << std::endl;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
