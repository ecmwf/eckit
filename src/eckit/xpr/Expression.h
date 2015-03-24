/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Expression.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Expression_h
#define eckit_xpr_Expression_h

/// @todo look into currying / binding -- add2 = curry ( add , 2 )
/// @todo list --  list 1 2
/// @todo std::map
/// @todo fold
/// @todo comparison operator Equal()(v,2) -- returns what?
/// @todo unary operator
/// @todo operator returning real
/// @todo operator returning multiple outputs
/// @todo how to support multiple implementations ( MKL, CuBLAS, etc. )
/// @todo create a expression tree Visitor class that takes an operation as parameter

//--------------------------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/serialisation/Streamable.h"

//--------------------------------------------------------------------------------------------

namespace eckit_test { // For unit tests....
    class TestExp;
}

namespace eckit {

class JSON;

namespace xpr {

//--------------------------------------------------------------------------------------------

const int CodeFormat = 42; // To use with format() in Log..

//--------------------------------------------------------------------------------------------

class Value;
class List;
class Expression;
class Scope;

typedef double real_t;
typedef int64_t integer_t;

typedef SharedPtr<List>        ListPtr;
typedef SharedPtr<Expression>  ExpPtr;

typedef std::vector< ExpPtr > args_t;

//--------------------------------------------------------------------------------------------

/// Expression Error
class Error: public eckit::Exception {
public:
    Error(const eckit::CodeLocation& loc, const std::string& s) : Exception( s, loc ) {}
};

//--------------------------------------------------------------------------------------------

class Optimiser;

class Expression : public Owned, public Streamable {

public: // methods

    struct Swap {};

    static const char * nodeName() { return "xpr::Expression"; }

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Expression();

    /// Contructor taking a list of parameters
    /// handle the setup of the parameters themselves
    Expression( const args_t& args );
    Expression(args_t& args, Swap);

    Expression(Expression&&);

    Expression(Stream& s);

    virtual ~Expression();

    Expression& operator=(Expression&&);

    ExpPtr self()       { return ExpPtr(this); }
    ExpPtr self() const { return ExpPtr( const_cast<Expression*>( this ) ); }

    template< typename T >
    SharedPtr<T> as()
    {
        return SharedPtr<T>( dynamic_cast<T*>( this ) );
    }

    friend std::ostream& operator<<( std::ostream& os, const Expression& v);
    friend JSON& operator<<(JSON& s, const Expression& v);

    ExpPtr eval(bool optimize = true) const;
    ExpPtr eval(ExpPtr, bool optimize = true ) const;
    ExpPtr eval(ExpPtr, ExpPtr, bool optimize = true) const;

    ExpPtr eval( Scope& ) const;

    size_t arity() const { return args_.size(); }

    std::string str() const;
    std::string code() const;
    std::string json() const;

    // Used to bind undef() and lambda parameters
    virtual ExpPtr resolve(Scope &) const;

    static const eckit::ClassSpec& classSpec() { return classSpec_; }

    ExpPtr args( size_t i) const;

public: // virtual methods

    virtual std::string factoryName() const { return "xpr::expression"; }
    virtual const char * typeName() const = 0;
    virtual std::string signature() const = 0;

    ExpPtr optimise(bool doit, size_t depth = 0) const;

    /// Adds support for count()
    virtual bool countable() const { return false; }
    virtual size_t count() const { return 0; }
    virtual ExpPtr optimise(size_t) const;

protected: // members

    // From Streamable
    virtual void encode(eckit::Stream& s) const;

    virtual void asCode( std::ostream& ) const;
    virtual void asJSON( JSON& ) const;

    void printArgs(std::ostream& ) const;
    void printArgs(JSON&) const;

    // args_ are read-only, use these methods instead

    void push_back(const ExpPtr& e)      { args_.push_back(e);    }
    args_t::const_iterator begin() const { return args_.begin(); }
    args_t::const_iterator end() const   { return args_.end();   }

    size_t size() const { return args_.size(); }

    const args_t& args() const { return args_; }

    ExpPtr args(size_t i, Scope& ctx , bool eval) const;

private: // members

    args_t args_;     ///< parameters of this expression

private: // methods

    virtual ExpPtr evaluate( Scope& ) const = 0;

    virtual void print( std::ostream& ) const = 0;

    virtual ExpPtr cloneWith(args_t&) const = 0;

    bool optimiseArgs(args_t&) const;

    // For unit tests....
    friend class eckit_test::TestExp;

    static  ClassSpec classSpec_;
};

//--------------------------------------------------------------------------------------------

template < typename T >
typename T::value_t extract( const ExpPtr& x )
{
    return T::extract( x );
}

} // namespace xpr
} // namespace eckit

#endif
