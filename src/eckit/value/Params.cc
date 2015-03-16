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

void encode( const Params& p, Stream& s )
{
    p.self_->encode_(s);
}

std::map<std::string, Params::factory_t> Params::factories_;

//------------------------------------------------------------------------------------------------------

CompositeParams::CompositeParams() : plist_()
{
}

CompositeParams::CompositeParams(const Params::List& plist) : plist_(plist)
{
}

CompositeParams& CompositeParams::push_front(const Params& p)
{
    plist_.push_front(p);
    return *this;
}

CompositeParams& CompositeParams::push_back(const Params& p)
{
    plist_.push_back(p);
    return *this;
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


void encode( const CompositeParams& p, Stream& s )
{
    s << p.plist_.size();
    for( Params::List::const_iterator citr = p.plist_.begin(); citr != p.plist_.end(); ++citr )
        encode(*citr, s);
}

Params::Factory<CompositeParams> compositeParamsFactory;

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

void encode(const ValueParams& p, Stream& s)
{
    s << p.props_;
}

Params::Factory<ValueParams> valueParamsFactory;

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
        return get( p.p_, key.substr(p.scope_.length()) );
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

void encode( const ScopeParams& p, Stream &s )
{
    s << p.p_;
    s << p.scope_;
}

Params::Factory<ScopeParams> scopeParamsFactory;

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

void encode( const UnScopeParams& p, Stream &s )
{
    s << p.p_;
    s << p.scope_;
}

Params::Factory<UnScopeParams> unScopeParamsFactory;

//------------------------------------------------------------------------------------------------------

} // namespace eckit
