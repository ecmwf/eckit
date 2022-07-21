/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/BackendRegistry.h"

#include "eckit/eckit.h"


namespace eckit {
namespace linalg {


const char* sparse_backend_default() {
    return
#ifdef eckit_HAVE_EIGEN
        "eigen"
#else
        "generic"
#endif
        ;
}


const char* dense_backend_default() {
    return
#if eckit_HAVE_MKL // always defined: 0 or 1
        "mkl"
#elif defined(eckit_HAVE_LAPACK)
        "lapack"
#elif defined(eckit_HAVE_EIGEN)
        "eigen"
#else
        "generic"
#endif
        ;
}


}  // namespace linalg
}  // namespace eckit
