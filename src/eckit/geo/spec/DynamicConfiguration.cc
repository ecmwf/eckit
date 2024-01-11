/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geo/spec/DynamicConfiguration.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/value/Value.h"


namespace eckit::geo::spec {


namespace {


const LocalConfiguration __empty_configuration;
const eckit::Value __empty_root;


}  // namespace


DynamicConfiguration::DynamicConfiguration() :
    DynamicConfiguration(__empty_configuration) {}


DynamicConfiguration::DynamicConfiguration(const Configuration& config) :
    Configuration(__empty_root), config_(config) {}


void DynamicConfiguration::hide(const std::string& name) {
    hide_.insert(name);
}


void DynamicConfiguration::unhide(const std::string& name) {
    hide_.erase(name);
}


void DynamicConfiguration::push_back(Configuration* config) {
    ASSERT(config != nullptr);
    after_.emplace_back(config);
}


void DynamicConfiguration::push_front(Configuration* config) {
    ASSERT(config != nullptr);
    before_.emplace_back(config);
}


void DynamicConfiguration::print(std::ostream& out) const {
    const auto* sep = "";
    out << "DynamicConfiguration[hide[";
    for (const auto& name : hide_) {
        out << sep << name;
        sep = ",";
    }

    sep = "";
    out << "],config=" << config_ << ",before[";
    for (const auto& config : before_) {
        out << sep << *config;
        sep = ",";
    }

    sep = "";
    out << ",after[";
    for (const auto& config : after_) {
        out << sep << *config;
        sep = ",";
    }
    out << "]]";
}


}  // namespace eckit::geo::spec
