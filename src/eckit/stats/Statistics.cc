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
//include "eckit/thread/Once.h"

#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"


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


Statistics::CounterUnary::CounterUnary(const data::MIRField& field) :
    count_(0),
    missing_(0),
    missingValue_(field.missingValue()),
    hasMissing_(field.hasMissing()) {
}


bool Statistics::CounterUnary::missingValue(const double& v) {
    count_++;
    if (hasMissing_ && missingValue_ == v) {
        missing_++;
        return false;
    }
    return true;

}


Statistics::CounterBinary::CounterBinary(const data::MIRField& field1, const data::MIRField& field2) :
    counter1_(field1),
    counter2_(field2),
    missing1_(0),
    missing2_(0) {
}


bool Statistics::CounterBinary::missingValues(const double& v1, const double& v2) {
    bool miss1 = counter1_.missingValue(v1);
    bool miss2 = counter2_.missingValue(v2);
    if (miss1 || miss2) {
        (miss1 && miss2 ? missing2_ : missing1_)++;
        return true;
    }
    return false;
}


size_t Statistics::CounterBinary::count() const {
    ASSERT(counter1_.count() == counter2_.count());
    return counter1_.count();
}


Statistics::CountOutside::CountOutside(const double &upperLimit, const double &lowerLimit) :
    lowerLimit_(lowerLimit),
    upperLimit_(upperLimit),
    hasLowerLimit_(lowerLimit == lowerLimit),
    hasUpperLimit_(upperLimit == upperLimit) {
}


size_t Statistics::CountOutside::count() const {
    return count_;
}


void Statistics::CountOutside::count(const double &v) {
    if (hasLowerLimit_ && v < lowerLimit_) {
        ++count_;
    }
    if (hasUpperLimit_ && v > upperLimit_) {
        ++count_;
    }
}


StatisticsFactory::StatisticsFactory(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);

    eckit::AutoLock< eckit::Mutex > lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw eckit::SeriousBug("StatisticsFactory: duplicated '" + name + "'");
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

