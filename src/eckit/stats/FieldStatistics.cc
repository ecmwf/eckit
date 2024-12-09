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


#include "eckit/stats/FieldStatistics.h"

#include <cmath>
#include <map>
#include <ostream>

#include "eckit/log/JSON.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/stats/util/Mutex.h"


namespace eckit::stats {


static util::recursive_mutex* local_mutex                = nullptr;
static std::map<std::string, FieldStatisticsFactory*>* m = nullptr;
static util::once_flag once;
static void init() {
    local_mutex = new util::recursive_mutex();
    m           = new std::map<std::string, FieldStatisticsFactory*>();
}


FieldStatistics::FieldStatistics(const Parametrisation& /*unused*/) {}


void FieldStatistics::json_tv(eckit::JSON& j, const std::string& type, double value) {
    j.startObject();
    j << "type" << type;
    if (!std::isnan(value)) {
        j << "value" << value;
    }
    j.endObject();
}


FieldStatistics::~FieldStatistics() = default;


FieldStatisticsFactory::FieldStatisticsFactory(const std::string& name) : name_(name) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        throw SeriousBug("FieldStatisticsFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


FieldStatisticsFactory::~FieldStatisticsFactory() {
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


void FieldStatisticsFactory::list(std::ostream& out) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
    out << std::endl;
}


FieldStatistics* FieldStatisticsFactory::build(const std::string& name, const Parametrisation& param) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    Log::debug() << "FieldStatisticsFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "FieldStatisticsFactory: unknown '" << name << "', choices are: ");
        throw SeriousBug("FieldStatisticsFactory: unknown '" + name + "'");
    }

    return j->second->make(param);
}


}  // namespace eckit::stats
