/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Exp.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Exp_h
#define eckit_maths_Exp_h

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "eckit/eckit.h"

/// @todo look into currying / binding -- add2 = curry ( add , 2 )
/// @todo list --  list 1 2
/// @todo map
/// @todo fold
/// @todo comparison operator Equal()(v,2) -- returns what?
/// @todo unary operator
/// @todo operator returning scalar
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

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "eckit/exception/Exceptions.h"

//--------------------------------------------------------------------------------------------

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

#if 0
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << Here() << " " << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//--------------------------------------------------------------------------------------------

class Value;
class List;
class Expr;

typedef double scalar_t;

typedef boost::shared_ptr<Value>      ValPtr;
typedef boost::shared_ptr<List>       ListPtr;
typedef boost::shared_ptr<Expr> ExpPtr;

typedef std::vector< ExpPtr > args_t;

typedef std::deque< ExpPtr > context_t;

//--------------------------------------------------------------------------------------------

/// Expr Error
class Error: public eckit::Exception {
public:
    Error(const eckit::CodeLocation& loc, const string& s) : Exception( s, loc ) {}
};

//--------------------------------------------------------------------------------------------

class Expr :
    public boost::enable_shared_from_this<Expr>,
    private boost::noncopyable {

public: // methods

    static std::string class_name() { return "Exp"; }

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Expr();

    /// Contructor taking a list of parameters
    /// handle the setup of the parameters themselves
    Expr( const args_t& args );

    virtual ~Expr();

    ExpPtr self() { return shared_from_this(); }

    template< typename T >
    boost::shared_ptr<T> as()
    {
        return boost::dynamic_pointer_cast<T,Expr>( shared_from_this() );
    }

    friend std::ostream& operator<<( std::ostream& os, const Expr& v)
    {
        v.print(os);
        return os;
    }

    ValPtr eval();
    ValPtr eval( ExpPtr );
    ValPtr eval( ExpPtr, ExpPtr );
    ValPtr eval( const args_t& );

    size_t arity() const { return args_.size(); }

    ExpPtr param( const size_t& i, context_t* ctx = 0 ) const;

    void param( const size_t& i, ExpPtr p );

    std::string str() const;

public: // virtual methods

    virtual std::string type_name() const = 0;
    virtual ValPtr evaluate( context_t& ) = 0;
    virtual ExpPtr optimise() = 0;
    virtual ExpPtr clone() = 0;
    virtual void print( std::ostream& ) const = 0;
    virtual std::string signature() const = 0;
    virtual std::string ret_signature() const = 0;

protected: // members

    args_t args_;     ///< parameters of this expression

};

//--------------------------------------------------------------------------------------------

class Undef : public Expr {
public: //  methods

    static std::string class_name() { return "Undef"; }
    static std::string sig() { return "?"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == Undef::sig(); }

    Undef();

    virtual ~Undef();

    virtual ExpPtr optimise() { return shared_from_this(); }

    virtual ValPtr evaluate( context_t& ctx );

    virtual ExpPtr clone();

    virtual std::string type_name() const { return Undef::class_name(); }
    virtual std::string signature() const { return Undef::sig(); }
    virtual std::string ret_signature() const { return Undef::sig(); }

    virtual void print( std::ostream& o ) const { o << Undef::sig(); }

};

ExpPtr undef();

//--------------------------------------------------------------------------------------------

class Param : public Expr {

    size_t n_;

public: //  methods

    static std::string class_name() { return "Param"; }
    static std::string sig() { return "_"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == Param::sig(); }

    Param(size_t);

    virtual ~Param();

    size_t n() const { return n_; }

    virtual ExpPtr optimise() { return shared_from_this(); }

    virtual ValPtr evaluate( context_t& ctx );

    virtual ExpPtr clone();

    virtual std::string type_name() const { return Param::class_name(); }
    virtual std::string signature() const { return Param::sig(); }
    virtual std::string ret_signature() const { return Param::sig(); }

    virtual void print( std::ostream& o ) const;

};

ExpPtr parameter(size_t n);

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
