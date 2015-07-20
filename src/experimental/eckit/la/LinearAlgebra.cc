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
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "experimental/eckit/la/LinearAlgebra.h"

namespace eckit {
namespace la {

namespace {

static pthread_once_t once = PTHREAD_ONCE_INIT;

static Mutex *local_mutex = 0;

typedef std::map<std::string, const LinearAlgebra *> BackendMap;
static BackendMap *m = 0;

static std::string *currentBackend = 0;

static void init() {
    local_mutex = new Mutex;
    m = new BackendMap;
    currentBackend = new std::string("generic");
}

}  // anonymous namespace

//-----------------------------------------------------------------------------

const LinearAlgebra &LinearAlgebra::backend() {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(local_mutex);

    BackendMap::const_iterator it = m->find(*currentBackend);
    if (it == m->end()) {
        eckit::Log::error() << "No Linear algebra backend named [" << *currentBackend << "]" << std::endl;
        eckit::Log::error() << "Linear algebra backends are:" << std::endl;
        for (it = m->begin() ; it != m->end() ; ++it)
            eckit::Log::error() << "   " << (*it).first << std::endl;
        throw BadParameter("Linear algebra backend " + *currentBackend + " not available.", Here());
    }
    Log::debug() << "Using LinearAlgebra backend " << it->first << std::endl;
    return *(it->second);
}

void LinearAlgebra::backend(const std::string &name) {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(local_mutex);
    *currentBackend = name;
    Log::info() << "Setting LinearAlgebra backend to " << backend() << std::endl;
}

void LinearAlgebra::list(std::ostream &out) {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(local_mutex);

    const char *sep = "";
    for (BackendMap::const_iterator it = m->begin() ; it != m->end() ; ++it) {
        out << sep << it->first;
        sep = ", ";
    }
}

LinearAlgebra::LinearAlgebra(const std::string &name) {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
