/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/LinearAlgebraDense.h"

#include <map>

#include "eckit/eckit.h"
#include "eckit/linalg/BackendRegistry.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


namespace {


#ifdef eckit_HAVE_EIGEN
static const auto* defaultBackend = "eigen";
#else
static const auto* defaultBackend = "generic";
#endif

static pthread_once_t once                           = PTHREAD_ONCE_INIT;
static BackendRegistry<LinearAlgebraDense>* backends = nullptr;


static void init() {
    backends = new BackendRegistry<LinearAlgebraDense>(defaultBackend, "ECKIT_LINEAR_ALGEBRA_DENSE_BACKEND");
}


}  // namespace


//-----------------------------------------------------------------------------


const LinearAlgebraDense& LinearAlgebraDense::backend(const std::string& name) {
    pthread_once(&once, init);
    if (!name.empty()) {
        backends->backend(name);
    }
    return backends->find();
}


const LinearAlgebraDense& LinearAlgebraDense::getBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends->find(name);
}


bool LinearAlgebraDense::hasBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends->has(name);
}


std::ostream& LinearAlgebraDense::list(std::ostream& out) {
    pthread_once(&once, init);
    return backends->list(out);
}


LinearAlgebraDense::LinearAlgebraDense(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);
    backends->add(name, this);
}


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
