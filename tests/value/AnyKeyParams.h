/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
