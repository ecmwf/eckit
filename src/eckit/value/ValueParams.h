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

#ifndef eckit_value_ValueParams_H
#define eckit_value_ValueParams_H

#include "eckit/value/Params.h"
#include "eckit/value/Properties.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

class ValueParams {

public: // methods

    ValueParams() : props_() {}
    ValueParams( const Properties& p ) : props_(p) {}
    ValueParams( Stream& s ) : props_(s) {}

    ValueParams& set( const Params::key_t& k, const Params::value_t& v );

    static const char* className() { return "ValueParams"; }

private: // methods

    friend Params::value_t get( const ValueParams& p, const Params::key_t& key );
    friend void print( const ValueParams& p, std::ostream& s );
    friend void encode( const ValueParams& p, Stream& s );

	Properties& props() { return props_; }

private: // members

    Properties props_;
};

//----------------------------------------------------------------------------

} // namespace eckit

#endif
