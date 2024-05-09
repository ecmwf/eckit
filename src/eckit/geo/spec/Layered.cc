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
#include "eckit/log/JSON.h"
#include "eckit/value/Value.h"


namespace eckit::geo::spec {


static const Custom EMPTY;


Layered::Layered() : Layered(EMPTY) {}


Layered::Layered(const Spec& spec) : spec_(spec) {}


void Layered::hide(const std::string& name) {
    hide_.insert(name);
}


void Layered::unhide(const std::string& name) {
    hide_.erase(name);
}


void Layered::push_back(Spec* spec) {
    ASSERT(spec != nullptr);
    back_.emplace_back(spec);
}


void Layered::push_front(Spec* spec) {
    ASSERT(spec != nullptr);
    front_.emplace_back(spec);
}


void Layered::print(std::ostream& out) const {
    JSON j(out);
    j.startObject();

    j << "hide";
    j.startList();
    for (const auto& name : hide_) {
        j << name;
    }
    j.endList();

    j << "before";
    j.startList();
    for (const auto& spec : front_) {
        spec->json(j);
    }
    j.endList();

    j << "spec";
    spec_.json(j);

    j << "after";
    j.startList();
    for (const auto& spec : back_) {
        spec->json(j);
    }
    j.endList();

    j.endObject();
}


void Layered::json(JSON&) const {
    NOTIMP;
}


}  // namespace eckit::geo::spec
