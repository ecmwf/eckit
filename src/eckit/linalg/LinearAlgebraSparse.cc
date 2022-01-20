/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/LinearAlgebraSparse.h"

#include "eckit/linalg/BackendRegistry.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


static pthread_once_t once                            = PTHREAD_ONCE_INIT;
static BackendRegistry<LinearAlgebraSparse>* backends = nullptr;


static void init() {
    backends = new BackendRegistry<LinearAlgebraSparse>(backend_default(), "ECKIT_LINEAR_ALGEBRA_SPARSE_BACKEND");
}


//-----------------------------------------------------------------------------


const LinearAlgebraSparse& LinearAlgebraSparse::backend(const std::string& name) {
    pthread_once(&once, init);
    if (!name.empty()) {
        backends->backend(name);
    }
    return backends->find();
}


const LinearAlgebraSparse& LinearAlgebraSparse::getBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends->find(name);
}


bool LinearAlgebraSparse::hasBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends->has(name);
}


std::ostream& LinearAlgebraSparse::list(std::ostream& out) {
    pthread_once(&once, init);
    return backends->list(out);
}


const std::string& LinearAlgebraSparse::name() {
    return backends->name();
}


LinearAlgebraSparse::LinearAlgebraSparse(const std::string& name) {
    pthread_once(&once, init);
    backends->add(name, this);
}


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
