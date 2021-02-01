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


#include "mir/stats/Method.h"

#include <map>
#include <mutex>
#include <ostream>

#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace stats {


static std::recursive_mutex* local_mutex        = nullptr;
static std::map<std::string, MethodFactory*>* m = nullptr;
static std::once_flag once;
static void init() {
    local_mutex = new std::recursive_mutex();
    m           = new std::map<std::string, MethodFactory*>();
}


Method::Method(const param::MIRParametrisation& parametrisation) : parametrisation_(parametrisation) {}


Method::~Method() = default;


MethodFactory::MethodFactory(const std::string& name) : name_(name) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("MethodFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


MethodFactory::~MethodFactory() {
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


void MethodFactory::list(std::ostream& out) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
    out << std::endl;
}


Method* MethodFactory::build(const std::string& name, const param::MIRParametrisation& params) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    Log::debug() << "MethodFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "MethodFactory: unknown '" << name << "', choices are:\n");
        throw exception::SeriousBug("MethodFactory: unknown '" + name + "'");
    }

    return j->second->make(params);
}


}  // namespace stats
}  // namespace mir
