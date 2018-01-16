/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/eckit_config.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/config/LibEcKit.h"

#include "eckit/linalg/LinearAlgebra.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

namespace {

#ifdef ECKIT_HAVE_EIGEN
    static const char* defaultBackend = "eigen";
#else
    static const char* defaultBackend = "generic";
#endif

static pthread_once_t once = PTHREAD_ONCE_INIT;

class BackendRegistry {

public:
    typedef std::map<std::string, const LinearAlgebra *> Map;

    BackendRegistry() : default_(defaultBackend) {
        char* envBackend = ::getenv("ECKIT_LINEAR_ALGEBRA_BACKEND");
        if(envBackend) {
            default_ = envBackend;
        }
        // std::cout << "Default Linear Algebra backend: " << default_ << std::endl;
    }

    void backend(const std::string& name);

    const LinearAlgebra& find() const;
    const LinearAlgebra& find(const std::string& name) const;

    void list(std::ostream &out) const;

    void add(const std::string& name, LinearAlgebra* backend);

private: // members

    Map map_;
    std::string default_;
    mutable Mutex mutex_;
};

static BackendRegistry* backends = 0;

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

const LinearAlgebra&BackendRegistry::find() const {
    return find(default_);
}

const LinearAlgebra&BackendRegistry::find(const std::string& name) const {
    AutoLock<Mutex> lock(mutex_);

    BackendRegistry::Map::const_iterator it = map_.find(name);
    if (it == map_.end()) {
        eckit::Log::error() << "No Linear algebra backend named [" << name << "]" << std::endl;
        eckit::Log::error() << "Linear algebra backends are:" << std::endl;
        for (it = map_.begin() ; it != map_.end() ; ++it)
            eckit::Log::error() << "   " << (*it).first << std::endl;
        throw BadParameter("Linear algebra backend " + name + " not available.", Here());
    }
    Log::debug<LibEcKit>() << "Using LinearAlgebra backend " << it->first << std::endl;
    return *(it->second);
}

void BackendRegistry::list(std::ostream& out) const {
    AutoLock<Mutex> lock(mutex_);
    const char *sep = "";
    for (Map::const_iterator it = map_.begin() ; it != map_.end() ; ++it) {
        out << sep << it->first;
        sep = ", ";
    }
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

void LinearAlgebra::backend(const std::string &name) {
    pthread_once(&once, init);
    backends->backend(name);
    Log::debug<LibEcKit>() << "Setting LinearAlgebra backend to " << name << std::endl;
}

void LinearAlgebra::list(std::ostream &out) {
    pthread_once(&once, init);
    backends->list(out);
}

void LinearAlgebra::dsptd(const Vector &, const SparseMatrix &, const Vector &, SparseMatrix &) const
{
    NOTIMP;
}

LinearAlgebra::LinearAlgebra(const std::string &name):
    name_(name) {
    pthread_once(&once, init);
    backends->add(name, this);
}

const std::string& LinearAlgebra::name() const {
    return name_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit
