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

#include <mutex>
#include <sstream>

#include "eckit/parser/YAMLParser.h"

#include "mir/param/SimpleParametrisation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace stats {


static std::once_flag once;
static std::recursive_mutex* local_mutex              = nullptr;
static std::map<std::string, DistributionFactory*>* m = nullptr;
static void init() {
    local_mutex = new std::recursive_mutex();
    m           = new std::map<std::string, DistributionFactory*>();
}


Distribution::Distribution() = default;


Distribution::~Distribution() = default;


DistributionFactory::DistributionFactory(const std::string& name) : name_(name) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        std::ostringstream oss;
        oss << "DistributionFactory: duplicate '" << name << "'";
        throw exception::SeriousBug(oss.str());
    }

    (*m)[name] = this;
}


DistributionFactory::~DistributionFactory() {
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


Distribution* DistributionFactory::build(const std::string& name) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    // parse <name>[{<yaml-arguments>}] (arguments are optional)
    auto braces = name.find('{');
    ASSERT(braces == std::string::npos || name.back() == '}');

    std::string key  = name.substr(0, braces);
    std::string yaml = name.substr(braces);

    param::SimpleParametrisation args;
    if (!yaml.empty()) {
        eckit::ValueMap map = eckit::YAMLParser::decodeString(yaml);
        for (const auto& kv : map) {
            kv.second.isDouble()   ? args.set(kv.first, kv.second.as<double>())
            : kv.second.isNumber() ? args.set(kv.first, kv.second.as<long long>())
                                   : args.set(kv.first, kv.second.as<std::string>());
        }
    }

    Log::debug() << "DistributionFactory: looking for '" << key << "'" << std::endl;

    auto j = m->find(key);
    if (j == m->end()) {
        list(Log::error() << "DistributionFactory: unknown '" << key << "', choices are: ");
        Log::warning() << std::endl;
    }

    return j->second->make(args);
}


void DistributionFactory::list(std::ostream& out) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace stats
}  // namespace mir
