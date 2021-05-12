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


#include "mir/stats/Distribution.h"

#include <sstream>

#include "eckit/parser/YAMLParser.h"

#include "mir/param/SimpleParametrisation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Mutex.h"
#include "mir/util/ValueMap.h"


namespace mir {
namespace stats {


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
        throw exception::SeriousBug(oss.str());
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

    auto j = m->find(key);
    if (j == m->end()) {
        list(Log::error() << "DistributionFactory: unknown '" << key << "', choices are: ");
        Log::warning() << std::endl;
    }

    param::SimpleParametrisation args;
    if (!yaml.empty()) {
        util::ValueMap map(eckit::YAMLParser::decodeString(yaml));
        map.set(args);
    }

    return j->second->make(args);
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


}  // namespace stats
}  // namespace mir
