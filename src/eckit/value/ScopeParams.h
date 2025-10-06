/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date March 2015

#ifndef eckit_value_ScopeParams_H
#define eckit_value_ScopeParams_H

#include "eckit/value/Params.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

/// Wraps the parameters within a given scope

class ScopeParams {

public:  // methods

    ScopeParams(const Params::key_t& scope_key, const Params& p);
    ScopeParams(Stream& s);

    static const char* className() { return "eckit::ScopeParams"; }

private:  // methods

    friend Params::value_t getValue(const ScopeParams& p, const Params::key_t& key);
    friend void print(const ScopeParams& p, std::ostream& s);
    friend void encode(const ScopeParams& p, Stream& s);

private:  // members

    Params::key_t scope_;
    Params p_;
};

//----------------------------------------------------------------------------

}  // namespace eckit

#endif
