/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "AnyKeyParams.h"

using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

namespace eckit_test {

//----------------------------------------------------------------------------------------------------------------------

AnyKeyParams::AnyKeyParams(const std::string& payload) :
    payload_(payload) {}

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
