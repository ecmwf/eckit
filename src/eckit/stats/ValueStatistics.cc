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


#include "mir/stats/ValueStatistics.h"

#include <map>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/config/LibMir.h"


namespace mir {
namespace stats {


static eckit::Mutex* local_mutex                         = nullptr;
static std::map<std::string, ValueStatisticsFactory*>* m = nullptr;
static pthread_once_t once                               = PTHREAD_ONCE_INIT;
static void init() {
    local_mutex = new eckit::Mutex();
    m           = new std::map<std::string, ValueStatisticsFactory*>();
}


void ValueStatistics::operator()(const double&) {
    NOTIMP;  // ensure specialisation
}


void ValueStatistics::operator()(const float&) {
    NOTIMP;  // ensure specialisation
}


ValueStatisticsFactory::ValueStatisticsFactory(const std::string& name) : name_(name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw eckit::SeriousBug("ValueStatisticsFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


ValueStatisticsFactory::~ValueStatisticsFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


void ValueStatisticsFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
    out << std::endl;
}


ValueStatistics* ValueStatisticsFactory::build(const std::string& name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "ValueStatisticsFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(eckit::Log::error() << "No ValueStatisticsFactory '" << name << "', choices are:\n");
        throw eckit::SeriousBug("No ValueStatisticsFactory '" + name + "'");
    }

    return (*j).second->make();
}


}  // namespace stats
}  // namespace mir
