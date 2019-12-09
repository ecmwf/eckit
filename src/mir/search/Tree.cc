/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @author Peter Bispham
/// @date Apr 2015


#include "mir/search/Tree.h"

#include <iostream>
#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/config/LibMir.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace search {


Tree::~Tree() = default;


Tree::Tree(const repres::Representation& r) : itemCount_(r.numberOfPoints()) {
    ASSERT(itemCount_ > 0);
}


void Tree::build(std::vector<PointValueType>&) {
    std::ostringstream os;
    os << "Tree::build() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Tree::insert(const PointValueType&) {
    std::ostringstream os;
    os << "Tree::insert() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Tree::statsPrint(std::ostream&, bool) {
    std::ostringstream os;
    os << "Tree::statsPrint() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Tree::statsReset() {
    std::ostringstream os;
    os << "Tree::statsReset() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


Tree::PointValueType Tree::nearestNeighbour(const Point&) {
    std::ostringstream os;
    os << "Tree::nearestNeighbour() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


std::vector<Tree::PointValueType> Tree::kNearestNeighbours(const Point&, size_t) {
    std::ostringstream os;
    os << "Tree::kNearestNeighbours() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


std::vector<Tree::PointValueType> Tree::findInSphere(const Point&, double) {
    std::ostringstream os;
    os << "Tree::findInSphere() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Tree::ready() const {
    std::ostringstream os;
    os << "Tree::ready() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Tree::commit() {
    std::ostringstream os;
    os << "Tree::commit() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Tree::print(std::ostream& out) const {
    out << "Tree[]" << std::endl;
}


void Tree::lock() {
    // Empty
}


void Tree::unlock() {
    // Empty
}


// -----------------------------------------------------------------------------


namespace {
static pthread_once_t once = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex = nullptr;
static std::map<std::string, TreeFactory*>* m = nullptr;
static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<std::string, TreeFactory*>();
}
} // (anonymous namespace)


TreeFactory::TreeFactory(const std::string& name) : name_(name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    auto j = m->find(name);
    if (j == m->end()) {
        (*m)[name] = this;
        return;
    }

    throw eckit::SeriousBug("TreeFactory: duplicate '" + name + "'");
}


TreeFactory::~TreeFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


Tree* TreeFactory::build(const std::string& name,
                         const repres::Representation& r,
                         const param::MIRParametrisation& params) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "TreeFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(eckit::Log::error() << "TreeFactory: unknown '" << name << "', choices are: ");
        throw eckit::SeriousBug("TreeFactory: unknown '" + name + "'");
    }

    return (*j).second->make(r, params);
}


void TreeFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace search
}  // namespace mir
