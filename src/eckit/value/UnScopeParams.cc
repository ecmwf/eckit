/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/value/UnScopeParams.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

UnScopeParams::UnScopeParams( const Params::key_t& scope_key, const Params& p ) :
    scope_( scope_key + "." ),
    p_(p)
{
}

UnScopeParams::UnScopeParams( Stream& s ) : p_( Params::decode(s) )
{
    s >> scope_;
}

Params::value_t get( const UnScopeParams& p, const Params::key_t& key )
{
    return get( p.p_, p.scope_ + key );
}

void print( const UnScopeParams& p, std::ostream &s )
{
    print(p.p_, s);
}

void encode( const UnScopeParams& p, Stream &s )
{
    s << p.p_;
    s << p.scope_;
}

Params::Factory<UnScopeParams> unScopeParamsFactory;

//----------------------------------------------------------------------------

} // namespace eckit
