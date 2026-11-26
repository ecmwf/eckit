// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "AnyKeyParams.h"

using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

namespace eckit_test {

//----------------------------------------------------------------------------------------------------------------------

AnyKeyParams::AnyKeyParams(const std::string& payload) : payload_(payload) {}

AnyKeyParams::AnyKeyParams(Stream& s) {
    s >> payload_;
}

Params::value_t getValue(const AnyKeyParams&, const Params::key_t&) {
    return Params::value_t("foo");
}

void print(const AnyKeyParams&, std::ostream&) {}

void encode(const AnyKeyParams& p, Stream& s) {
    s << p.payload_;
}

//----------------------------------------------------------------------------------------------------------------------

Params::Factory<AnyKeyParams> anyKeyParamsFactory;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_test
