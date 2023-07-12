/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geometry/Area.h"

#include <map>
#include <ostream>

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


static pthread_once_t __once                           = PTHREAD_ONCE_INIT;
static Mutex* __mutex                                  = nullptr;
static std::map<Area::Type, AreaFactory*>* __factories = nullptr;


static void __init() {
    __mutex     = new Mutex;
    __factories = new std::map<Area::Type, AreaFactory*>();
}


Area* AreaFactory::build(const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    Area::Type type;
    ASSERT(config.get("type", type));

    return build(type, config);
}


Area* AreaFactory::build(const Area::Type& type,
                         const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(type); f != __factories->end()) {
        return f->second->make(config);
    }

    list(Log::error() << "AreaFactory: unknown '" << type << "', choices are: ");
    throw BadValue("AreaFactory: unknown '" + type + "'");
}


std::ostream& AreaFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const char* sep = "'";
    for (const auto& j : *__factories) {
        out << sep << j.first << '\'';
        sep = ", '";
    }

    return out;
}


AreaFactory::AreaFactory(const Area::Type& key) :
    key_(key) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        throw BadValue("AreaFactory: duplicate '" + key + "'");
    }

    (*__factories)[key] = this;
}


AreaFactory::~AreaFactory() {
    AutoLock<Mutex> lock(*__mutex);

    if (__factories != nullptr) {
        __factories->erase(key_);
    }
}


}  // namespace eckit::geometry
