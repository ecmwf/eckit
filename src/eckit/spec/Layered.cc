// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/spec/Layered.h"

#include <algorithm>

#include "eckit/log/JSON.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Exceptions.h"
#include "eckit/value/Value.h"


namespace eckit::spec {


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


bool Layered::only(const std::string& name) const {
    auto count_only = [&](const auto& c) { return c->only(name); };
    return 1 == (spec_.only(name) ? 1 : 0) +                                   //
                    std::count_if(front_.begin(), front_.end(), count_only) +  //
                    std::count_if(back_.begin(), back_.end(), count_only);
}


void Layered::json(JSON&) const {
    NOTIMP;
}


const Spec& Layered::spec(const std::string& name) const {
    for (const auto& spec : front_) {
        if (spec->has(name)) {
            return spec->spec(name);
        }
    }

    if (has(name)) {
        return Spec::spec(name);
    }

    for (const auto& spec : back_) {
        if (spec->has(name)) {
            return spec->spec(name);
        }
    }

    throw exception::SpecError("Layered::spec(" + name + ")", Here());
}


}  // namespace eckit::spec
