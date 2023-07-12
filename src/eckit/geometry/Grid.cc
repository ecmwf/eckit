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

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Domain.h"
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/detail/UnstructuredGrid.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


Grid::Grid(const Configuration& config) :
    bbox_(config) {}


Grid::Grid(const BoundingBox& bbox) :
    bbox_(bbox) {}


const BoundingBox& Grid::boundingBox() const {
    return bbox_;
}


static pthread_once_t __once;
static Mutex* __mutex                         = nullptr;
static std::map<std::string, GridFactory*>* m = nullptr;
static void __init() {
    __mutex = new Mutex;
    m       = new std::map<std::string, GridFactory*>();
}


GridFactory::GridFactory(const std::string& name) :
    name_(name) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (m->find(name) != m->end()) {
        throw SeriousBug("GridFactory: duplicate '" + name + "'");
    }

    (*m)[name] = this;
}


GridFactory::~GridFactory() {
    AutoLock<Mutex> lock(*__mutex);

    m->erase(name_);
}


const Grid* GridFactory::build(const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (Grid::UID uid; config.get("uid", uid)) {
        return build_from_uid(uid);
    }

    if (Grid::Name name; config.get("name", name)) {
        return config.has("area") ? config.has("rotation") ? build_from_name(name, new Grid::Area(config), ProjectionFactory::build(config)) : build_from_name(name, new Grid::Area(config)) : config.has("rotation") ? build_from_name(name, ProjectionFactory::build(config))
                                                                                                                                                                                                                      : build_from_name(name);
    }

    if (Grid::Type type; config.get("type", type)) {
        return config.has("area") ? config.has("rotation") ? build_from_type(type, new Grid::Area(config), ProjectionFactory::build(config)) : build_from_type(type, new Grid::Area(config)) : config.has("rotation") ? build_from_type(type, ProjectionFactory::build(config))
                                                                                                                                                                                                                      : build_from_type(type);
    }

    if (config.has("increments")) {
        Grid::Increments increments(config);
        return config.has("area") ? config.has("rotation") ? build_from_increments(increments, new Grid::Area(config), ProjectionFactory::build(config)) : build_from_increments(increments, new Grid::Area(config)) : config.has("rotation") ? build_from_increments(increments, ProjectionFactory::build(config))
                                                                                                                                                                                                                                              : build_from_increments(increments);
    }

    list(Log::error() << "GridFactory: cannot build grid, choices are: ");
    throw SeriousBug("GridFactory: cannot build grid");
}


const Grid* GridFactory::build_from_uid(const Grid::UID& uid) {
    NOTIMP;
}


const Grid* GridFactory::build_from_name(const Grid::Name&, Grid::Area*, Grid::Projection*) {
    NOTIMP;
}


const Grid* GridFactory::build_from_name(const Grid::Name&, Grid::Projection*) {
    NOTIMP;
}


const Grid* GridFactory::build_from_type(const Grid::Type&, Grid::Area*, Grid::Projection*) {
    NOTIMP;
}


const Grid* GridFactory::build_from_type(const Grid::Type&, Grid::Projection*) {
    NOTIMP;
}


const Grid* GridFactory::build_from_increments(const Increments&, Grid::Area*, Grid::Projection*) {
    NOTIMP;
}


const Grid* GridFactory::build_from_increments(const Increments&, Grid::Projection*) {
    NOTIMP;
}


void GridFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace eckit::geometry
