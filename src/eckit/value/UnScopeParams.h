/*
 * (C) Copyright 1996-2015 ECMWF.
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

#ifndef eckit_value_UnScopeParams_H
#define eckit_value_UnScopeParams_H

#include "eckit/value/Params.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

/// Searches the parameters within a given scope

class UnScopeParams {

public: // methods

    UnScopeParams( const Params::key_t& scope_key, const Params& p );
    UnScopeParams( Stream& s );

    static const char* className() { return "eckit::UnScopeParams"; }

private: // methods

    friend Params::value_t get( const UnScopeParams& p, const Params::key_t& key );
    friend void print( const UnScopeParams& p, std::ostream& s );
    friend void encode( const UnScopeParams& p, Stream& s );

private: // members

    Params::key_t scope_;
    Params p_;

};

//----------------------------------------------------------------------------

} // namespace eckit

#endif
