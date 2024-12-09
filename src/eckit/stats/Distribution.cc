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


#include "eckit/stats/Distribution.h"

#include <map>
#include <sstream>

#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/stats/util/Mutex.h"


namespace eckit::stats {


static util::once_flag once;
static util::recursive_mutex* local_mutex             = nullptr;
static std::map<std::string, DistributionFactory*>* m = nullptr;
static void init() {
    local_mutex = new util::recursive_mutex();
    m           = new std::map<std::string, DistributionFactory*>();
}


Distribution::Distribution() = default;


Distribution::~Distribution() = default;


DistributionFactory::DistributionFactory(const std::string& name) : name_(name) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        std::ostringstream oss;
        oss << "DistributionFactory: duplicate '" << name << "'";
        throw SeriousBug(oss.str());
    }

    (*m)[name] = this;
}


DistributionFactory::~DistributionFactory() {
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


Distribution* DistributionFactory::build(const std::string& name) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    // parse <name>[{<yaml-arguments>}] (arguments are optional)
    auto braces = name.find('{');
    ASSERT(braces == std::string::npos || name.back() == '}');

    auto key  = name.substr(0, braces);
    auto yaml = name.substr(braces);

    Log::debug() << "DistributionFactory: looking for '" << key << "'" << std::endl;

    if (auto j = m->find(key); j != m->end()) {
        return j->second->make(YAMLConfiguration{yaml});
    }

    list(Log::error() << "DistributionFactory: unknown '" << key << "', choices are: ");
    Log::warning() << std::endl;
}


void DistributionFactory::list(std::ostream& out) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace eckit::stats
