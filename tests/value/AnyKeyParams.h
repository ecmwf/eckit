// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file AnyKeyParams.h
/// @author Florian Rathgeber
/// @date March 2015

#ifndef eckit_test_AnyKeyParams_H
#define eckit_test_AnyKeyParams_H

#include "eckit/serialisation/Stream.h"
#include "eckit/value/Params.h"

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

struct AnyKeyParams {
    AnyKeyParams(const std::string& payload);
    AnyKeyParams(eckit::Stream& s);
    static const char* className() { return "AnyKeyParams"; }

private:

    friend void encode(const AnyKeyParams&, eckit::Stream&);
    std::string payload_;
};

eckit::Params::value_t getValue(const AnyKeyParams&, const eckit::Params::key_t&);

void print(const AnyKeyParams&, std::ostream&);
void encode(const AnyKeyParams& p, eckit::Stream& s);

//-----------------------------------------------------------------------------

}  // namespace eckit_test

#endif
