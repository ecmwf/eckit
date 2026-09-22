// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/linalg/LinearAlgebraSparse.h"

#include "eckit/eckit.h"
#include "eckit/linalg/BackendRegistry.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit::linalg {

//-----------------------------------------------------------------------------

static pthread_once_t once                            = PTHREAD_ONCE_INIT;
static BackendRegistry<LinearAlgebraSparse>* backends = nullptr;


static void init() {
    backends = new BackendRegistry<LinearAlgebraSparse>(
#if eckit_HAVE_EIGEN
        "eigen"
#else
        "generic"
#endif
        ,
        "ECKIT_LINEAR_ALGEBRA_SPARSE_BACKEND");
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

}  // namespace eckit::linalg
