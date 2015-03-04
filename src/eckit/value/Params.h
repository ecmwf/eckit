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

#include "eckit/value/Value.h"
#include "eckit/value/Properties.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

class Params {

public: // types

    typedef std::list<Params> List;
    typedef std::string  key_t;
    typedef eckit::Value value_t;

public: // methods

    template <typename T>
    explicit Params( T x ) : self_(new Model<T>(x)) {}

    ~Params() { delete self_; }

    Params& operator=( const Params& x )
    {
        Params tmp(x);
        std::swap(tmp, *this);
        return *this;
    }

    bool has( const key_t& key ) const;

    value_t operator[] ( const key_t& key ) const;

    friend value_t get( const Params& p, const key_t& key );

    friend void print( const Params& p, std::ostream& s );

private: // internal classes

    struct Concept {
        virtual ~Concept() {}
        virtual value_t get_( const key_t& key ) const = 0;
        virtual void print_( std::ostream& s ) const = 0;
    };

    template <typename T>
    struct Model : Concept
    {
        Model( T x ) : data_(x) {}
        virtual value_t get_( const key_t& key ) const {
            return get( data_, key );
        }
        virtual void print_( std::ostream& s ) const {
            print( data_, s );
        }
        T data_;
    };

private: // methods

    friend std::ostream& operator<<(std::ostream& s, const Params& p)
    {
        print(p, s);
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

    void push_front( const Params& p );
    void push_back( const Params& p );

    void pop_front() { plist_.pop_front(); }
    void pop_back()  { plist_.pop_back(); }

private: // methods

    friend Params::value_t get( const CompositeParams& p, const Params::key_t& key );
    friend void print( const CompositeParams& p, std::ostream& s );

private: // members

    Params::List plist_;
};

//------------------------------------------------------------------------------------------------------

class ValueParams {

public: // methods

    ValueParams() : props_() {}
    ValueParams( const eckit::Properties& p ) : props_(p) {}

    void set( const Params::key_t& k, const Params::value_t& v );

private: // methods

    friend Params::value_t get( const ValueParams& p, const Params::key_t& key );
    friend void print( const ValueParams& p, std::ostream& s );

	Properties& props() { return props_; }

private: // members

    eckit::Properties props_;
};

//-------------------------------------------------------------------------------------------

template < class Derived >
class DispatchParams {

public: // methods

    DispatchParams() {}

    template < typename T >
    friend Params::value_t get( const DispatchParams<T>& p, const Params::key_t& key );

protected: // members

    typedef Params::value_t ( Derived::* parametrizer_t ) ( const Params::key_t& ) const ;
    typedef std::map< std::string, parametrizer_t > store_t;

    store_t dispatch_;
};

//-------------------------------------------------------------------------------------------

/// Wraps the parameters within a given scope

class ScopeParams {

public: // methods

    ScopeParams( const Params::key_t& scope_key, const Params& p );

private: // methods

    friend Params::value_t get( const ScopeParams& p, const Params::key_t& key );
    friend void print( const ScopeParams& p, std::ostream& s );

private: // members

    Params::key_t scope_;
    Params p_;
};

//-------------------------------------------------------------------------------------------

/// Searches the parameters within a given scope

class UnScopeParams {

public: // methods

    UnScopeParams( const Params::key_t& scope_key, const Params& p );

private: // methods

    friend Params::value_t get( const UnScopeParams& p, const Params::key_t& key );
    friend void print( const UnScopeParams& p, std::ostream& s );

private: // members

    Params::key_t scope_;
    Params p_;

};

//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
