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
#include <ostream>
#include <type_traits>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/GridConfig.h"
#include "eckit/geometry/util/regex.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


Grid::Grid(const Configuration& config) :
    bbox_(config) {}


Grid::Grid(const area::BoundingBox& bbox) :
    bbox_(bbox) {}


const area::BoundingBox& Grid::boundingBox() const {
    throw NotImplemented("Grid::boundingBox", Here());
}


void Grid::boundingBox(const area::BoundingBox& bbox) {
    bbox_ = bbox;
}


size_t Grid::size() const {
    throw NotImplemented("Grid::size", Here());
}


bool Grid::includesNorthPole() const {
    throw NotImplemented("Grid::includesNorthPole", Here());
}


bool Grid::includesSouthPole() const {
    throw NotImplemented("Grid::includesSouthPole", Here());
}


bool Grid::isPeriodicWestEast() const {
    throw NotImplemented("Grid::isPeriodicWestEast", Here());
}


std::vector<Point> Grid::to_points() const {
    std::vector<Point> points;
    points.reserve(size());

    for (const auto& p : *this) {
        const auto& q = std::get<PointLonLat>(p);
        points.push_back(p);
    }

    return points;
}


std::pair<std::vector<double>, std::vector<double>> Grid::to_latlon() const {
    throw NotImplemented("Grid::to_latlon");
}


Ordering Grid::order() const {
    throw NotImplemented("Grid::order");
}


Renumber Grid::reorder(const PointLonLat&) const {
    throw NotImplemented("Grid::reorder");
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

    GridConfig::instance();

    if (std::string uid; config.get("uid", uid)) {
        return GridFactoryUID::build(uid);
    }

    if (std::string name; config.get("name", name)) {
        return GridFactoryName::build(name);
    }

    if (config.has("type")) {
        return GridFactoryType::build(config);
    }

    list(Log::error() << "Grid: cannot build grid, choices are: ");
    throw SeriousBug("Grid: cannot build grid");
}


void GridFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    GridConfig::instance();

    GridFactoryUID::list(out << "uid: ");
    out << std::endl;

    GridFactoryName::list(out << "name: ");
    out << std::endl;

    GridFactoryType::list(out << "type: ");
    out << std::endl;
}


void GridFactory::json(JSON& j) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    GridConfig::instance();

    j.startObject();
    GridFactoryUID::json(j << "uid");
    GridFactoryName::json(j << "name");
    GridFactoryType::json(j << "type");
    j.endObject();
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
        std::unique_ptr<Configuration> config(j->second->config());
        return GridFactory::build(*config);
    }

    list(Log::error() << "Grid: unknown identifier '" << uid << "', choices are: ");
    throw SeriousBug("Grid: unknown identifier '" + uid + "'");
}


void GridFactoryUID::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    JSON j(out);
    json(j);
}


void GridFactoryUID::json(JSON& j) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    j.startObject();
    for (const auto& p : *__grid_uids) {
        j << p.first;

        std::unique_ptr<Configuration> config(p.second->config());
        j << *config;
    }
    j.endObject();
}


void GridFactoryUID::insert(const std::string& uid, MappedConfiguration* config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    struct Insert final : GridFactoryUID {
        Insert(const std::string& uid, MappedConfiguration* config) :
            GridFactoryUID(uid), config_(config) {}
        Configuration* config() override { return new MappedConfiguration(*config_); }
        const std::unique_ptr<MappedConfiguration> config_;
    };

    new Insert(uid, config);
}


GridFactoryName::GridFactoryName(const std::string& pattern, const std::string& example) :
    pattern_(pattern), example_(example) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (__grid_names->find(pattern) == __grid_names->end()) {
        (*__grid_names)[pattern] = this;
        return;
    }

    throw SeriousBug("Grid: duplicate name '" + pattern + "'");
}


const Grid* GridFactoryName::build(const std::string& name) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    const auto end = __grid_names->cend();

    auto i = end;
    for (auto j = __grid_names->cbegin(); j != end; ++j) {
        if (util::regex_match(j->second->pattern_, name)) {
            if (i != end) {
                throw SeriousBug("Grid: name '" + name + "' matches '" + i->second->pattern_ + "' and '" + j->second->pattern_ + "'");
            }
            i = j;
        }
    }

    if (i != end) {
        std::unique_ptr<Configuration> config(i->second->config(name));
        return GridFactory::build(*config);
    }

    list(Log::error() << "Grid: unknown name '" << name << "', choices are: ");
    throw SeriousBug("Grid: unknown name '" + name + "'");
}


void GridFactoryName::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    JSON j(out);
    json(j);
}


void GridFactoryName::json(JSON& j) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    j.startObject();
    for (const auto& p : *__grid_names) {
        j << p.first;

        std::unique_ptr<Configuration> config(p.second->config());
        j << *config;
    }
    j.endObject();
}


void GridFactoryName::insert(const std::string& name, MappedConfiguration* config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    struct Insert final : GridFactoryName {
        Insert(const std::string& name, const std::string& example, MappedConfiguration* config) :
            GridFactoryName(name, example), config_(config) {}
        Configuration* config(const std::string&) const override { return new MappedConfiguration(*config_); }
        const std::unique_ptr<MappedConfiguration> config_;
    };

    ASSERT(config != nullptr);
    new Insert(name, config->getString("example", ""), config);
}


GridFactoryName::~GridFactoryName() {
    AutoLock<Mutex> lock(*__mutex);
    __grid_names->erase(pattern_);
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


const Grid* GridFactoryType::build(const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    std::string type;
    if (config.get("type", type)) {
        if (auto j = __grid_types->find(type); j != __grid_types->end()) {
            return j->second->make(config);
        }
    }

    list(Log::error() << "Grid: unknown type '" << type << "', choices are: ");
    throw SeriousBug("Grid: unknown type '" + type + "'");
}


void GridFactoryType::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    JSON j(out);
    json(j);
}


void GridFactoryType::json(JSON& j) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    j.startList();
    for (const auto& p : *__grid_types) {
        j << p.first;
    }
    j.endList();
}


}  // namespace eckit::geometry
