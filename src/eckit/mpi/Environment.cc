/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Environment.h"
#include "eckit/runtime/Context.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Comm& comm(const char* name)
{
    return Environment::instance().comm(name);
}

//----------------------------------------------------------------------------------------------------------------------

Comm& Environment::comm(const char* name) const {
    NOTIMP;
}

Environment& Environment::instance()
{
    NOTIMP;
}

Environment::Environment()
{
}

Environment::~Environment()
{
}

//----------------------------------------------------------------------------------------------------------------------

Operation sum() {
    return Environment::instance().sum();
}

Operation prod() {
    return Environment::instance().prod();
}

Operation max() {
    return Environment::instance().max();
}

Operation min() {
    return Environment::instance().min();
}

Operation maxloc() {
    return Environment::instance().maxloc();
}

Operation minloc() {
    return Environment::instance().minloc();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
