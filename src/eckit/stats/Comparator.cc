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


#include "mir/stats/Comparator.h"

#include <map>
#include <ostream>

#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Mutex.h"


namespace mir {
namespace stats {


static util::recursive_mutex* local_mutex           = nullptr;
static std::map<std::string, ComparatorFactory*>* m = nullptr;
static util::once_flag once;
static void init() {
    local_mutex = new util::recursive_mutex();
    m           = new std::map<std::string, ComparatorFactory*>();
}


Comparator::Comparator(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) :
    parametrisation1_(param1), parametrisation2_(param2) {}


Comparator::~Comparator() = default;


ComparatorFactory::ComparatorFactory(const std::string& name) : name_(name) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("ComparatorFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


ComparatorFactory::~ComparatorFactory() {
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


void ComparatorFactory::list(std::ostream& out) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
    out << std::endl;
}


Comparator* ComparatorFactory::build(const std::string& name, const param::MIRParametrisation& param1,
                                     const param::MIRParametrisation& param2) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    Log::debug() << "ComparatorFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "ComparatorFactory: unknown '" << name << "', choices are: ");
        throw exception::SeriousBug("ComparatorFactory: unknown '" + name + "'");
    }

    return j->second->make(param1, param2);
}


}  // namespace stats
}  // namespace mir
