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


#include "eckit/geometry/Figure.h"

#include <map>
#include <ostream>

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


Figure::Figure(const Configuration& config) /*:
    Figure(config.getDouble("a"), config.getDouble("b"))*/
{
}


static pthread_once_t __once                              = PTHREAD_ONCE_INIT;
static Mutex* __mutex                                     = nullptr;
static std::map<std::string, FigureFactory*>* __factories = nullptr;


static void __init() {
    __mutex     = new Mutex;
    __factories = new std::map<std::string, FigureFactory*>();
}


Figure* FigureFactory::build(const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    std::string type;
    ASSERT(config.get("type", type));

    return build(type, config);
}


Figure* FigureFactory::build(const std::string& type, const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(type); f != __factories->end()) {
        return f->second->make(config);
    }

    list(Log::error() << "FigureFactory: unknown '" << type << "', choices are: ");
    throw BadValue("FigureFactory: unknown '" + type + "'");
}


std::ostream& FigureFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    JSON j(out);
    j.startObject();

    j << "type";
    j.startList();
    for (const auto& p : *__factories) {
        j << p.first;
    }
    j.endList();

    j.endObject();

    return out;
}


FigureFactory::FigureFactory(const std::string& key) :
    key_(key) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        throw BadValue("FigureFactory: duplicate '" + key + "'");
    }

    (*__factories)[key] = this;
}


FigureFactory::~FigureFactory() {
    AutoLock<Mutex> lock(*__mutex);

    if (__factories != nullptr) {
        __factories->erase(key_);
    }
}


}  // namespace eckit::geometry
