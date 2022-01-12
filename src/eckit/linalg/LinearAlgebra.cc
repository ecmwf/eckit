/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <map>

#include "eckit/eckit.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "eckit/linalg/LinearAlgebra.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

namespace {

#ifdef eckit_HAVE_EIGEN
static const auto* defaultBackend = "eigen";
#else
static const auto* defaultBackend = "generic";
#endif

static pthread_once_t once = PTHREAD_ONCE_INIT;

class BackendRegistry {
public:
    using Map = std::map<std::string, const LinearAlgebra*>;

    BackendRegistry() :
        default_(defaultBackend) {
        auto* envBackend = ::getenv("ECKIT_LINEAR_ALGEBRA_BACKEND");
        if (envBackend) {
            default_ = envBackend;
        }
    }

    void backend(const std::string&);

    bool has(const std::string&) const;

    const LinearAlgebra& find() const;
    const LinearAlgebra& find(const std::string&) const;

    std::ostream& list(std::ostream&) const;

    void add(const std::string&, LinearAlgebra* backend);

private:  // members
    Map map_;
    std::string default_;
    mutable Mutex mutex_;
};

static BackendRegistry* backends = nullptr;

static void init() {
    backends = new BackendRegistry();
}

void BackendRegistry::backend(const std::string& name) {
    AutoLock<Mutex> lock(mutex_);

    if (map_.find(name) == map_.end()) {
        throw BadParameter("Invalid backend " + name, Here());
    }
    default_ = name;
}

const LinearAlgebra& BackendRegistry::find() const {
    return find(default_);
}

const LinearAlgebra& BackendRegistry::find(const std::string& name) const {
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

bool BackendRegistry::has(const std::string& name) const {
    AutoLock<Mutex> lock(mutex_);

    return map_.find(name) != map_.end();
}

std::ostream& BackendRegistry::list(std::ostream& out) const {
    AutoLock<Mutex> lock(mutex_);

    const auto* sep = "";
    for (auto backend : map_) {
        out << sep << backend.first;
        sep = ", ";
    }

    return out;
}

void BackendRegistry::add(const std::string& name, LinearAlgebra* backend) {
    AutoLock<Mutex> lock(mutex_);

    ASSERT(map_.find(name) == map_.end());
    map_[name] = backend;
}

}  // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------

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

void LinearAlgebra::list(std::ostream& out) {
    pthread_once(&once, init);
    backends->list(out);
}

LinearAlgebra::LinearAlgebra(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);
    backends->add(name, this);
}

LinearAlgebra::~LinearAlgebra() {}

const std::string& LinearAlgebra::name() const {
    return name_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
