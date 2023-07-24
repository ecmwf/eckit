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


#include "eckit/geometry/Grid.h"

#include <map>
#include <memory>
#include <sstream>
#include <type_traits>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


Grid::Grid(const Configuration& config) :
    bbox_(config) {}


Grid::Grid(const area::BoundingBox& bbox) :
    bbox_(bbox) {}


const area::BoundingBox& Grid::boundingBox() const {
    return bbox_;
}


static pthread_once_t __once;

static Mutex* __mutex                                        = nullptr;
static std::map<std::string, GridFactoryUID*>* __grid_uids   = nullptr;
static std::map<std::string, GridFactoryName*>* __grid_names = nullptr;
static std::map<std::string, GridFactoryType*>* __grid_types = nullptr;

static void __init() {
    __mutex      = new Mutex;
    __grid_uids  = new std::remove_reference<decltype(*__grid_uids)>::type;
    __grid_names = new std::remove_reference<decltype(*__grid_names)>::type;
    __grid_types = new std::remove_reference<decltype(*__grid_types)>::type;
}


const Grid* GridFactory::build(const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (std::string uid; config.get("uid", uid)) {
        return GridFactoryUID::build(uid);
    }

    if (std::string name; config.get("name", name)) {
        return config.has("area") ? config.has("rotation") ? GridFactoryName::build(name, AreaFactory::build(config), ProjectionFactory::build(config)) : GridFactoryName::build(name, AreaFactory::build(config)) : config.has("rotation") ? GridFactoryName::build(name, ProjectionFactory::build(config))
                                                                                                                                                                                                                                            : GridFactoryName::build(name);
    }

    if (std::string type; config.get("type", type)) {
        return config.has("area") ? config.has("rotation") ? GridFactoryType::build(type, AreaFactory::build(config), ProjectionFactory::build(config)) : GridFactoryType::build(type, AreaFactory::build(config)) : config.has("rotation") ? GridFactoryType::build(type, ProjectionFactory::build(config))
                                                                                                                                                                                                                                            : GridFactoryType::build(type);
    }

    list(Log::error() << "Grid: cannot build grid, choices are: ");
    throw SeriousBug("Grid: cannot build grid");
}


void GridFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    GridFactoryUID::list(out);
    GridFactoryName::list(out);
    GridFactoryType::list(out);
}


GridFactoryUID::GridFactoryUID(const std::string& uid) :
    uid_(uid) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (__grid_uids->find(uid) == __grid_uids->end()) {
        (*__grid_uids)[uid] = this;
        return;
    }

    throw SeriousBug("Grid: duplicate identifier '" + uid + "'");
}


GridFactoryUID::~GridFactoryUID() {
    AutoLock<Mutex> lock(*__mutex);
    __grid_uids->erase(uid_);
}


const Grid* GridFactoryUID::build(const std::string& uid) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto j = __grid_uids->find(uid); j != __grid_uids->end()) {
        return j->second->make();
    }

    list(Log::error() << "Grid: unknown identifier '" << uid << "', choices are: ");
    throw SeriousBug("Grid: unknown identifier '" + uid + "'");
}


void GridFactoryUID::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const char* sep = "";
    for (const auto& j : *__grid_uids) {
        out << sep << j.first;
        sep = ", ";
    }
}


GridFactoryName::GridFactoryName(const std::string& name) :
    name_(name) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (__grid_names->find(name) == __grid_names->end()) {
        (*__grid_names)[name] = this;
        return;
    }

    throw SeriousBug("Grid: duplicate name '" + name + "'");
}


const Grid* GridFactoryName::build(const std::string& name, Area* area, Projection* projection) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto j = __grid_names->find(name); j != __grid_names->end()) {
        NOTIMP;
        // return j->second->make(name, area, projection);
    }

    list(Log::error() << "Grid: unknown name '" << name << "', choices are: ");
    throw SeriousBug("Grid: unknown name '" + name + "'");
}


const Grid* GridFactoryName::build(const std::string& name, Projection* projection) {
    return build(name, nullptr, projection);
}


void GridFactoryName::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const char* sep = "";
    for (const auto& j : *__grid_names) {
        out << sep << j.first;
        sep = ", ";
    }
}


GridFactoryName::~GridFactoryName() {
    AutoLock<Mutex> lock(*__mutex);
    __grid_names->erase(name_);
}


GridFactoryType::GridFactoryType(const std::string& type) :
    type_(type) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (__grid_types->find(type) == __grid_types->end()) {
        (*__grid_types)[type] = this;
        return;
    }

    throw SeriousBug("Grid: duplicate type '" + type + "'");
}


GridFactoryType::~GridFactoryType() {
    AutoLock<Mutex> lock(*__mutex);
    __grid_types->erase(type_);
}


const Grid* GridFactoryType::build(const std::string& type, Area* area, Projection* projection) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto j = __grid_types->find(type); j != __grid_types->end()) {
        NOTIMP;
        // return j->second->make(type, area, projection);
    }

    list(Log::error() << "Grid: unknown type '" << type << "', choices are: ");
    throw SeriousBug("Grid: unknown type '" + type + "'");
}


const Grid* GridFactoryType::build(const std::string& type, Projection* projection) {
    return build(type, nullptr, projection);
}


void GridFactoryType::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const char* sep = "";
    for (const auto& j : *__grid_types) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace eckit::geometry
