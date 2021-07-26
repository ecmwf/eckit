/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/StringTools.h"

#include "eckit/value/ScopeParams.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ScopeParams::ScopeParams(const Params::key_t& scope_key, const Params& p) :
    scope_(scope_key + "."), p_(p) {}

ScopeParams::ScopeParams(Stream& s) :
    p_(Params::decode(s)) {
    s >> scope_;
}

Params::value_t getValue(const ScopeParams& p, const Params::key_t& key) {
    if (StringTools::startsWith(key, p.scope_)) {
        return getValue(p.p_, key.substr(p.scope_.length()));
    }
    else {
        return Params::value_t();
    }
}

void print(const ScopeParams& p, std::ostream& s) {
    print(p.p_, s);
}

void encode(const ScopeParams& p, Stream& s) {
    s << p.p_;
    s << p.scope_;
}

Params::Factory<ScopeParams> scopeParamsFactory;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
