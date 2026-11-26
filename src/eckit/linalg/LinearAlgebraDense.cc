// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/linalg/LinearAlgebraDense.h"

#include "eckit/eckit.h"
#include "eckit/linalg/BackendRegistry.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit::linalg {

//-----------------------------------------------------------------------------

static pthread_once_t once                           = PTHREAD_ONCE_INIT;
static BackendRegistry<LinearAlgebraDense>* backends = nullptr;


static void init() {
    backends = new BackendRegistry<LinearAlgebraDense>(
#if eckit_HAVE_MKL
        "mkl"
#elif eckit_HAVE_LAPACK
        "lapack"
#elif eckit_HAVE_EIGEN
        "eigen"
#else
        "generic"
#endif
        ,
        "ECKIT_LINEAR_ALGEBRA_DENSE_BACKEND");
}


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


const std::string& LinearAlgebraDense::name() {
    return backends->name();
}


LinearAlgebraDense::LinearAlgebraDense(const std::string& name) {
    pthread_once(&once, init);
    backends->add(name, this);
}


//-----------------------------------------------------------------------------

}  // namespace eckit::linalg
