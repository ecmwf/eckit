/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/DynamicConfiguration.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/value/Value.h"


namespace eckit {


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
    configs_.emplace_back(config);
}


void DynamicConfiguration::push_front(Configuration* config) {
    ASSERT(config != nullptr);
    configs_.emplace_front(config);
}


void DynamicConfiguration::print(std::ostream& out) const {
    const auto* sep = "";
    out << "DynamicConfiguration[hide[";
    for (const auto& name : hide_) {
        out << sep << name;
        sep = ",";
    }

    sep = "";
    out << "],config=" << config_ << ",configs[";
    for (const auto& config : configs_) {
        out << sep << *config;
        sep = ",";
    }
    out << "]]";
}


}  // namespace eckit
