/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date July 2014

#ifndef eckit_value_Params_H
#define eckit_value_Params_H

#include <list>

#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/value/Value.h"
#include "eckit/value/Properties.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

class Params : public eckit::Owned {

public: // types

    typedef eckit::SharedPtr<Params> Ptr;

    typedef std::list< Params::Ptr > List;

    typedef std::string  key_t;
    typedef eckit::Value value_t;

public: // methods

    virtual ~Params();

    Ptr self() { return Params::Ptr(this); }
    Ptr self() const { return Params::Ptr( const_cast<Params*>(this) ); }

    virtual value_t get( const key_t& key ) const = 0;

    value_t operator[] ( const key_t& key ) const;

    virtual void print(std::ostream& s) const = 0;

private: // methods

    friend std::ostream& operator<<(std::ostream& s, const Params& p) { p.print(s);  return s; }

};

//------------------------------------------------------------------------------------------------------

class CompositeParams : public Params {

public: // methods

    CompositeParams();
    CompositeParams( const Params::List& );

    virtual value_t get( const key_t& key ) const;

    void push_front( const Params::Ptr& p );
    void push_back( const Params::Ptr& p );

protected: // methods

    virtual void print(std::ostream& s) const;

private: // members

    Params::List plist_;

};

//------------------------------------------------------------------------------------------------------

class ValueParams : public Params {

public: // methods

    ValueParams() : props_() {}
    ValueParams( const eckit::Properties& p ) : props_(p) {}

    virtual value_t get( const key_t& key ) const;

    void set( const key_t& k, const value_t& v );

protected: // methods

    virtual void print(std::ostream& s) const;

protected: // members

    eckit::Properties props_;

};

//-------------------------------------------------------------------------------------------

template < class Derived >
class DispatchParams : public Params {

public: // methods

    DispatchParams() {}

public: // methods

    virtual value_t get( const key_t& key ) const
    {
        typename store_t::const_iterator i = dispatch_.find(key);
        if( i != dispatch_.end() )
        {
            parametrizer_t fptr = i->second;
            const Derived* pobj = static_cast<const Derived*>(this);
            return (pobj->*fptr)( key );
        }
        else
            return Params::value_t();
    }

protected: // methods

    virtual void print(std::ostream& s) const {}

protected: // members

    typedef Params::value_t ( Derived::* parametrizer_t ) ( const key_t& ) const ;
    typedef std::map< std::string, parametrizer_t > store_t;

    store_t dispatch_;

};

//-------------------------------------------------------------------------------------------

/// Wraps the parameters within a given scope

class ScopeParams : public Params {

public: // methods

    ScopeParams( const key_t& scope_key, const Params::Ptr& p );

    virtual value_t get( const key_t& key ) const;

protected: // methods

    virtual void print(std::ostream& s) const;

private: // members

    key_t scope_;
    Params::Ptr p_;

};

//-------------------------------------------------------------------------------------------

/// Searches the parameters within a given scope

class UnScopeParams : public Params {

public: // methods

    UnScopeParams( const key_t& scope_key, const Params::Ptr& p );

    virtual value_t get( const key_t& key ) const;

protected: // methods

    virtual void print(std::ostream& s) const;

private: // members

    key_t scope_;
    Params::Ptr p_;

};

//-------------------------------------------------------------------------------------------

class RuntimeParams : public Params {

public: // methods

    RuntimeParams( Params** runtime );

    virtual value_t get( const key_t& key ) const;

protected: // methods

    virtual void print(std::ostream& s) const;

private: // members

    Params** runtime_;

};

//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
