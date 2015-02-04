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

Params::~Params()
{
}

bool Params::has(const Params::key_t &key) const
{
    return !get(key).isNil();
}

Params::value_t Params::operator[]( const Params::key_t& key ) const
{
    value_t v = get(key);
    if( v.isNil() )
        throw BadParameter("Params does not contain key: " + key, Here());
    return v;
}

//------------------------------------------------------------------------------------------------------

CompositeParams::CompositeParams() : plist_() 
{
}

CompositeParams::CompositeParams(const Params::List& plist) : plist_(plist) 
{
}

CompositeParams::value_t CompositeParams::get( const key_t& key ) const
{
    for( Params::List::const_iterator citr = plist_.begin(); citr != plist_.end(); ++citr )
    {
        Value v = (*citr)->get(key);
        if( !v.isNil() )
            return v;
    }
    return Value();
}

void CompositeParams::push_front(const Params::Ptr& p)
{
    plist_.push_front(p);
}

void CompositeParams::push_back(const Params::Ptr& p)
{
    plist_.push_back(p);
}

void CompositeParams::print(std::ostream &s) const
{
    for( Params::List::const_iterator citr = plist_.begin(); citr != plist_.end(); ++citr )
        (*citr)->print(s);
}

//------------------------------------------------------------------------------------------------------

Params::value_t ValueParams::get( const key_t& key ) const
{
    return props_.get(key); // returns Value Nil if doesn't exist
}

void ValueParams::set(const Params::key_t& k, const Params::value_t& v)
{
    props_.set(k,v);
}

void ValueParams::print(std::ostream &s) const
{
    s << props_;
}

//------------------------------------------------------------------------------------------------------

ScopeParams::ScopeParams(const Params::key_t& scope_key, const Params::Ptr& p ) :
    scope_( scope_key + "." ),
    p_(p)
{
    ASSERT(p_);
}

Params::value_t ScopeParams::get(const key_t& key ) const
{
    if( StringTools::startsWith(key, scope_) )
    {
        return p_->get( key );
    }
    else
    {
    	return value_t();
    }
}

void ScopeParams::print(std::ostream &s) const
{
    p_->print(s);
}

//------------------------------------------------------------------------------------------------------

UnScopeParams::UnScopeParams(const key_t& scope_key, const Params::Ptr& p ) :
    scope_( scope_key + "." ),
    p_(p)
{
    ASSERT(p_);
}

Params::value_t UnScopeParams::get(const key_t& key ) const
{
    return p_->get( scope_ + key );
}

void UnScopeParams::print(std::ostream &s) const
{
    p_->print(s);
}

//------------------------------------------------------------------------------------------------------

} // namespace eckit
