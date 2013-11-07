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

/// @todo rename params_ to args_
/// @todo move args_ to Exp


//--------------------------------------------------------------------------------------------

#if 1
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << Here() << " " << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//--------------------------------------------------------------------------------------------

class Value;
class Func;
class Expression;

typedef double scalar_t;

typedef boost::shared_ptr<Value> ValPtr;
typedef boost::shared_ptr<Expression> ExpPtr;

typedef std::vector< ExpPtr > args_t;

//--------------------------------------------------------------------------------------------

/// Expression Error
class Error: public eckit::Exception {
public:
    Error(const eckit::CodeLocation& loc, const string& s) : Exception( s, loc ) {}
};

//--------------------------------------------------------------------------------------------

class Expression :
    public boost::enable_shared_from_this<Expression>,
    private boost::noncopyable {

public: // methods

    static std::string class_name() { return "Exp"; }

    virtual ~Expression();

    ExpPtr self() { return shared_from_this(); }

    template< typename T >
    boost::shared_ptr<T> as()
    {
        return boost::dynamic_pointer_cast<T,Expression>( shared_from_this() );
    }

    friend std::ostream& operator<<( std::ostream& os, const Expression& v)
    {
        v.print(os);
        return os;
    }

    ValPtr eval()
    {
//        return reduce()->evaluate();
          return evaluate();
    }

public: // virtual methods

    virtual std::string type_name() const = 0;
    virtual ValPtr evaluate() = 0;
    virtual ExpPtr reduce() = 0;
    virtual void print( std::ostream& ) const = 0;
    virtual std::string signature() const = 0;
    virtual std::string ret_signature() const = 0;

    virtual size_t arity() const = 0;
    virtual ExpPtr param( const size_t& ) const = 0;
    virtual void param( const size_t&, ExpPtr p ) = 0;

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
