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

#include "eckit/serialisation/Stream.h"
#include "eckit/value/Value.h"
#include "eckit/value/Properties.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

class Params {

public: // types

    typedef std::list<Params> List;
    typedef std::string  key_t;
    typedef Value value_t;

public: // methods

    template <typename T>
    explicit Params( const T& x ) : self_(new Model<T>(x)) {}

    explicit Params( const Params& x ) : self_(x.self_->copy_()) {}

    ~Params() { delete self_; }

    Params& operator=( Params x )
    {
        std::swap(x.self_, this->self_);
        return *this;
    }

    bool has( const key_t& key ) const;

    value_t operator[] ( const key_t& key ) const;

    friend value_t get( const Params& p, const key_t& key );

    friend void print( const Params& p, std::ostream& s );

    friend void encode( const Params& p, Stream& s );

private: // internal classes

    struct Concept {
        virtual ~Concept() {}
        virtual Concept* copy_() const = 0;
        virtual value_t get_( const key_t& key ) const = 0;
        virtual void print_( std::ostream& s ) const = 0;
        virtual void encode_( Stream& s ) const = 0;
    };

    template <typename T>
    struct Model : Concept
    {
        Model( T x ) : data_(x) {}
        virtual Concept* copy_() const {
            return new Model(data_);
        }
        virtual value_t get_( const key_t& key ) const {
            return get( data_, key );
        }
        virtual void print_( std::ostream& s ) const {
            print( data_, s );
        }
        virtual void encode_( Stream& s ) const {
            encode( data_, s );
        }
        T data_;
    };

private: // methods

    friend std::ostream& operator<<(std::ostream& s, const Params& p)
    {
        print(p, s);
        return s;
    }

    friend Stream& operator<<(Stream& s, const Params& p)
    {
        encode(p, s);
        return s;
    }

private: // members

    const Concept* self_;
};

//------------------------------------------------------------------------------------------------------

class CompositeParams {

public: // methods

    CompositeParams();
    CompositeParams( const Params::List& );

    CompositeParams& push_front( const Params& p );
    CompositeParams& push_back( const Params& p );

    CompositeParams& pop_front() { plist_.pop_front(); return *this; }
    CompositeParams& pop_back()  { plist_.pop_back(); return *this; }

    static const char* className() { return "CompositeParams"; }

private: // methods

    friend Params::value_t get( const CompositeParams& p, const Params::key_t& key );
    friend void print( const CompositeParams& p, std::ostream& s );
    friend void encode( const CompositeParams& p, Stream& s );

private: // members

    Params::List plist_;
};

//------------------------------------------------------------------------------------------------------

class ValueParams {

public: // methods

    ValueParams() : props_() {}
    ValueParams( const Properties& p ) : props_(p) {}

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

//-------------------------------------------------------------------------------------------

template < class Derived >
class DispatchParams {

public: // methods

    DispatchParams() {}

    static const char* className() { return "DispatchParams"; }

    template < typename T >
    friend Params::value_t get( const DispatchParams<T>& p, const Params::key_t& key );
    template < typename T >
    friend void print( const DispatchParams<T>&, std::ostream& );
    template < typename T >
    friend void encode( const DispatchParams<T>&, Stream& );

protected: // members

    typedef Params::value_t ( Derived::* parametrizer_t ) ( const Params::key_t& ) const ;
    typedef std::map< std::string, parametrizer_t > store_t;

    store_t dispatch_;
};

template < class Derived >
Params::value_t get( const DispatchParams<Derived>& p, const Params::key_t& key )
{
    typename DispatchParams<Derived>::store_t::const_iterator i = p.dispatch_.find(key);
    if( i != p.dispatch_.end() )
    {
        typename DispatchParams<Derived>::parametrizer_t fptr = i->second;
        const Derived* pobj = static_cast<const Derived*>(&p);
        return (pobj->*fptr)( key );
    }
    else
        return Params::value_t();
}

template < class Derived >
void print( const DispatchParams<Derived>&, std::ostream& )
{
}

template < class Derived >
void encode( const DispatchParams<Derived>&, Stream& )
{
    NOTIMP;
}

//-------------------------------------------------------------------------------------------

/// Wraps the parameters within a given scope

class ScopeParams {

public: // methods

    ScopeParams( const Params::key_t& scope_key, const Params& p );

    static const char* className() { return "ScopeParams"; }

private: // methods

    friend Params::value_t get( const ScopeParams& p, const Params::key_t& key );
    friend void print( const ScopeParams& p, std::ostream& s );
    friend void encode( const ScopeParams& p, Stream& s );

private: // members

    Params::key_t scope_;
    Params p_;
};

//-------------------------------------------------------------------------------------------

/// Searches the parameters within a given scope

class UnScopeParams {

public: // methods

    UnScopeParams( const Params::key_t& scope_key, const Params& p );

    static const char* className() { return "UnScopeParams"; }

private: // methods

    friend Params::value_t get( const UnScopeParams& p, const Params::key_t& key );
    friend void print( const UnScopeParams& p, std::ostream& s );
    friend void encode( const UnScopeParams& p, Stream& s );

private: // members

    Params::key_t scope_;
    Params p_;

};

//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
