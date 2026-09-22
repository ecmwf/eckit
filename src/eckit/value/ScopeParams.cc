// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/value/ScopeParams.h"

#include "eckit/utils/StringTools.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ScopeParams::ScopeParams(const Params::key_t& scope_key, const Params& p) : scope_(scope_key + "."), p_(p) {}

ScopeParams::ScopeParams(Stream& s) : p_(Params::decode(s)) {
    s >> scope_;
}

Params::value_t getValue(const ScopeParams& p, const Params::key_t& key) {
    if (StringTools::startsWith(key, p.scope_)) {
        return getValue(p.p_, key.substr(p.scope_.length()));
    }
    return Params::value_t();
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
