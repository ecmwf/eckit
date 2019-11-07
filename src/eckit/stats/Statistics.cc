/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/Statistics.h"

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
static std::map< std::string, StatisticsFactory* > *m = nullptr;
static pthread_once_t once = PTHREAD_ONCE_INIT;


static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map< std::string, StatisticsFactory* >();
}


}  // (anonymous namespace)


Statistics::Statistics(const param::MIRParametrisation& parametrisation) :
    parametrisation_(parametrisation) {
}


Statistics::~Statistics() = default;


StatisticsFactory::StatisticsFactory(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw eckit::SeriousBug("StatisticsFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


StatisticsFactory::~StatisticsFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


void StatisticsFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
    out << std::endl;
}


Statistics* StatisticsFactory::build(const std::string& name, const param::MIRParametrisation& params) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "StatisticsFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(eckit::Log::error() << "No StatisticsFactory '" << name << "', choices are:\n");
        throw eckit::SeriousBug("No StatisticsFactory '" + name + "'");
    }

    return (*j).second->make(params);
}


}  // namespace stats
}  // namespace mir

