/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/StringTools.h"

#include "eckit/value/Params.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

bool Params::has(const Params::key_t &key) const
{
    return !get(*this, key).isNil();
}

Params::value_t Params::operator[]( const Params::key_t& key ) const
{
    value_t v = get(*this, key);
    if( v.isNil() )
        throw BadParameter("Params does not contain key: " + key, Here());
    return v;
}

Params::value_t get( const Params& p, const Params::key_t& key )
{
    return p.self_->get_(key);
}

void print( const Params& p, std::ostream& s )
{
    p.self_->print_(s);
}

//------------------------------------------------------------------------------------------------------

CompositeParams::CompositeParams() : plist_() 
{
}

CompositeParams::CompositeParams(const Params::List& plist) : plist_(plist) 
{
}

void CompositeParams::push_front(const Params& p)
{
    plist_.push_front(p);
}

void CompositeParams::push_back(const Params& p)
{
    plist_.push_back(p);
}

Params::value_t get( const CompositeParams& p, const Params::key_t& key )
{
    for( Params::List::const_iterator citr = p.plist_.begin(); citr != p.plist_.end(); ++citr )
    {
        Value v = get(*citr, key);
        if( !v.isNil() )
            return v;
    }
    return Value();
}

void print( const CompositeParams& p, std::ostream& s )
{
    for( Params::List::const_iterator citr = p.plist_.begin(); citr != p.plist_.end(); ++citr )
        print(*citr, s);
}

//------------------------------------------------------------------------------------------------------

ValueParams & ValueParams::set(const Params::key_t& k, const Params::value_t& v)
{
    props_.set(k,v);
    return *this;
}

Params::value_t get( const ValueParams& p, const Params::key_t& key )
{
    return p.props_.get(key); // returns Value Nil if doesn't exist
}

void print(const ValueParams & p, std::ostream &s)
{
    s << p.props_;
}

//------------------------------------------------------------------------------------------------------

ScopeParams::ScopeParams(const Params::key_t& scope_key, const Params & p ) :
    scope_( scope_key + "." ),
    p_(p)
{
}

Params::value_t get( const ScopeParams& p, const Params::key_t& key )
{
    if( StringTools::startsWith(key, p.scope_) )
    {
        return get( p.p_, key );
    }
    else
    {
        return Params::value_t();
    }
}

void print( const ScopeParams& p, std::ostream &s )
{
    print(p.p_, s);
}

//------------------------------------------------------------------------------------------------------

UnScopeParams::UnScopeParams( const Params::key_t& scope_key, const Params& p ) :
    scope_( scope_key + "." ),
    p_(p)
{
}

Params::value_t get( const UnScopeParams& p, const Params::key_t& key )
{
    return get( p.p_, p.scope_ + key );
}

void print( const UnScopeParams& p, std::ostream &s )
{
    print(p.p_, s);
}

//------------------------------------------------------------------------------------------------------

} // namespace eckit
