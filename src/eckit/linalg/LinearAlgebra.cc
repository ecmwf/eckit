/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/LinearAlgebra.h"

#include <map>

#include "eckit/config/LibEcKit.h"
#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


namespace {


#ifdef eckit_HAVE_EIGEN
static const auto* defaultBackend       = "eigen";
static const auto* defaultDenseBackend  = "eigen";
static const auto* defaultSparseBackend = "eigen";
#else
static const auto* defaultBackend       = "generic";
static const auto* defaultDenseBackend  = "generic";
static const auto* defaultSparseBackend = "generic";
#endif

static pthread_once_t once = PTHREAD_ONCE_INIT;


template <typename LA>
class BackendRegistry {
public:
    BackendRegistry(const char* default_backend, const char* env_var) :
        default_(default_backend) {
        auto* envBackend = ::getenv(env_var);
        if (envBackend) {
            default_ = envBackend;
        }
    }

    void backend(const std::string&);

    bool has(const std::string&) const;

    const LA& find() const;
    const LA& find(const std::string&) const;

    std::ostream& list(std::ostream&) const;

    void add(const std::string&, LA* backend);

private:  // members
    std::map<std::string, const LA*> map_;
    std::string default_;
    mutable Mutex mutex_;
};


static BackendRegistry<LinearAlgebra>* backends              = nullptr;
static BackendRegistry<LinearAlgebraDense>* backends_dense   = nullptr;
static BackendRegistry<LinearAlgebraSparse>* backends_sparse = nullptr;


static void init() {
    backends        = new BackendRegistry<LinearAlgebra>(defaultBackend, "ECKIT_LINEAR_ALGEBRA_BACKEND");
    backends_dense  = new BackendRegistry<LinearAlgebraDense>(defaultDenseBackend, "ECKIT_LINEAR_ALGEBRA_DENSE_BACKEND");
    backends_sparse = new BackendRegistry<LinearAlgebraSparse>(defaultSparseBackend, "ECKIT_LINEAR_ALGEBRA_SPARSE_BACKEND");
}


template <typename LA>
void BackendRegistry<LA>::backend(const std::string& name) {
    AutoLock<Mutex> lock(mutex_);

    if (map_.find(name) == map_.end()) {
        throw BadParameter("Invalid backend " + name, Here());
    }
    default_ = name;
}


template <typename LA>
const LA& BackendRegistry<LA>::find() const {
    return find(default_);
}


template <typename LA>
const LA& BackendRegistry<LA>::find(const std::string& name) const {
    AutoLock<Mutex> lock(mutex_);

    auto it = map_.find(name);
    if (it == map_.end()) {
        auto msg = "No Linear algebra backend named [" + name + "].";
        list(Log::error() << msg + "\n Linear algebra backends are: ") << std::endl;
        throw BadParameter(msg, Here());
    }

    Log::debug<LibEcKit>() << "Using LinearAlgebra backend " << it->first << std::endl;
    return *(it->second);
}


template <typename LA>
bool BackendRegistry<LA>::has(const std::string& name) const {
    AutoLock<Mutex> lock(mutex_);

    return map_.find(name) != map_.end();
}


template <typename LA>
std::ostream& BackendRegistry<LA>::list(std::ostream& out) const {
    AutoLock<Mutex> lock(mutex_);

    const auto* sep = "";
    for (auto backend : map_) {
        out << sep << backend.first;
        sep = ", ";
    }

    return out;
}


template <typename LA>
void BackendRegistry<LA>::add(const std::string& name, LA* backend) {
    AutoLock<Mutex> lock(mutex_);

    ASSERT(map_.find(name) == map_.end());
    map_[name] = backend;
}


}  // anonymous namespace


//-----------------------------------------------------------------------------


const LinearAlgebra& LinearAlgebra::backend() {
    pthread_once(&once, init);
    return backends->find();
}


const LinearAlgebra& LinearAlgebra::getBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends->find(name);
}


bool LinearAlgebra::hasBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends->has(name);
}


void LinearAlgebra::backend(const std::string& name) {
    pthread_once(&once, init);
    backends->backend(name);
    Log::debug<LibEcKit>() << "Setting LinearAlgebra backend to " << name << std::endl;
}


std::ostream& LinearAlgebra::list(std::ostream& out) {
    pthread_once(&once, init);
    return backends->list(out);
}


LinearAlgebra::LinearAlgebra(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);
    backends->add(name, this);
}


//-----------------------------------------------------------------------------


const LinearAlgebraDense& LinearAlgebraDense::backend() {
    pthread_once(&once, init);
    return backends_dense->find();
}


const LinearAlgebraDense& LinearAlgebraDense::getBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends_dense->find(name);
}


bool LinearAlgebraDense::hasBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends_dense->has(name);
}


void LinearAlgebraDense::backend(const std::string& name) {
    pthread_once(&once, init);
    backends_dense->backend(name);
    Log::debug<LibEcKit>() << "Setting LinearAlgebraDense backend to " << name << std::endl;
}


std::ostream& LinearAlgebraDense::list(std::ostream& out) {
    pthread_once(&once, init);
    return backends_dense->list(out);
}


LinearAlgebraDense::LinearAlgebraDense(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);
    backends_dense->add(name, this);
}


//-----------------------------------------------------------------------------


const LinearAlgebraSparse& LinearAlgebraSparse::backend() {
    pthread_once(&once, init);
    return backends_sparse->find();
}


const LinearAlgebraSparse& LinearAlgebraSparse::getBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends_sparse->find(name);
}


bool LinearAlgebraSparse::hasBackend(const std::string& name) {
    pthread_once(&once, init);
    return backends_sparse->has(name);
}


void LinearAlgebraSparse::backend(const std::string& name) {
    pthread_once(&once, init);
    backends_sparse->backend(name);
    Log::debug<LibEcKit>() << "Setting LinearAlgebraSparse backend to " << name << std::endl;
}


std::ostream& LinearAlgebraSparse::list(std::ostream& out) {
    pthread_once(&once, init);
    return backends_sparse->list(out);
}


LinearAlgebraSparse::LinearAlgebraSparse(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);
    backends_sparse->add(name, this);
}


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
