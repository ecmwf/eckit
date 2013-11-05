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

//-----------------------------------------------------------------------------

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

typedef double scalar_t;

class Var;
class Exp;

typedef boost::shared_ptr<Var> VarPtr;
typedef boost::shared_ptr<Exp> ExpPtr;

//--------------------------------------------------------------------------------------------

class Exp : public boost::enable_shared_from_this<Exp>,
            private boost::noncopyable {
public:

    static std::string class_name() { return "Exp"; }

    virtual std::string type_name() const = 0;

    virtual ~Exp();

    virtual VarPtr eval() = 0;
    virtual ExpPtr optimise() = 0;
    virtual ExpPtr reduce() = 0;

    virtual size_t arity() const = 0;
    virtual ExpPtr param( const size_t& ) const = 0;
    virtual void param( const size_t&, ExpPtr p ) = 0;

    ExpPtr self() { return shared_from_this(); }

    template< typename T >
    boost::shared_ptr<T> as() { return boost::dynamic_pointer_cast<T,Exp>( shared_from_this() ); }

    virtual void print( std::ostream& ) const = 0;
    virtual std::string signature() const = 0;

    friend std::ostream& operator<<( std::ostream& os, const Exp& v) { v.print(os); return os; }

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
