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


#include "eckit/geo/Iterator.h"

#include <map>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Projection.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geo {


static pthread_once_t __once                                              = PTHREAD_ONCE_INIT;
static Mutex* __mutex                                                     = nullptr;
static std::map<IteratorFactory::key_type, IteratorFactory*>* __factories = nullptr;


static void __init() {
    __mutex     = new Mutex;
    __factories = new std::map<IteratorFactory::key_type, IteratorFactory*>();
}


Iterator::Iterator(Projection* projection) :
    projection_(projection),
    valid_(true) {}


void Iterator::print(std::ostream& out) const {
    out << "Iterator[valid?"
        << valid_ << ",point=" << point_ << "]";
}


Iterator* IteratorFactory::build(const IteratorFactory::key_type& key, const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        return f->second->make(config);
    }

    list(Log::error() << "IteratorFactory: unknown '" << key << "', choices are: ");
    throw BadValue("IteratorFactory: unknown '" + key + "'");
}


std::ostream& IteratorFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const char* sep = "'";
    for (const auto& j : *__factories) {
        out << sep << j.first << '\'';
        sep = ", '";
    }

    return out;
}


IteratorFactory::IteratorFactory(const key_type& key) :
    key_(key) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        throw BadValue("IteratorFactory: duplicate '" + key + "'");
    }

    (*__factories)[key] = this;
}


IteratorFactory::~IteratorFactory() {
    AutoLock<Mutex> lock(*__mutex);

    if (__factories != nullptr) {
        __factories->erase(key_);
    }
}


}  // namespace eckit::geo
