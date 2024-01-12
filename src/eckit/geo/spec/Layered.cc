/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geo/spec/Layered.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/value/Value.h"


namespace eckit::geo::spec {


static const Custom __empty_spec;


Layered::Layered() :
    Layered(__empty_spec) {}


Layered::Layered(const Spec& spec) :
    spec_(spec) {}


void Layered::hide(const std::string& name) {
    hide_.insert(name);
}


void Layered::unhide(const std::string& name) {
    hide_.erase(name);
}


void Layered::push_back(Spec* spec) {
    ASSERT(spec != nullptr);
    after_.emplace_back(spec);
}


void Layered::push_front(Spec* spec) {
    ASSERT(spec != nullptr);
    before_.emplace_back(spec);
}


void Layered::print(std::ostream& out) const {
    const auto* sep = "";
    out << "Layered[hide[";
    for (const auto& name : hide_) {
        out << sep << name;
        sep = ",";
    }

    sep = "";
    out << "],spec=" << spec_ << ",before[";
    for (const auto& spec : before_) {
        out << sep << *spec;
        sep = ",";
    }

    sep = "";
    out << ",after[";
    for (const auto& spec : after_) {
        out << sep << *spec;
        sep = ",";
    }
    out << "]]";
}


void Layered::json(JSON&) const {
    NOTIMP;
}


}  // namespace eckit::geo::spec
