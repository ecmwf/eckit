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
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/stats/Comparator.h"

#include <map>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/config/LibMir.h"


namespace mir {
namespace stats {


namespace {


static eckit::Mutex* local_mutex = nullptr;
static std::map< std::string, ComparatorFactory* > *m = nullptr;
static pthread_once_t once = PTHREAD_ONCE_INIT;


static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map< std::string, ComparatorFactory* >();
}


}  // (anonymous namespace)


Comparator::Comparator(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) :
    parametrisation1_(param1),
    parametrisation2_(param2) {
}


Comparator::~Comparator() = default;


ComparatorFactory::ComparatorFactory(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw eckit::SeriousBug("ComparatorFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


ComparatorFactory::~ComparatorFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


void ComparatorFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
    out << std::endl;
}


Comparator* ComparatorFactory::build(const std::string& name, const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "ComparatorFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(eckit::Log::error() << "No ComparatorFactory '" << name << "', choices are:");
        throw eckit::SeriousBug("No ComparatorFactory '" + name + "'");
    }

    return (*j).second->make(param1, param2);
}


}  // namespace stats
}  // namespace mir

