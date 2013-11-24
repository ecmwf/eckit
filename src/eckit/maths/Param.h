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

#ifndef eckit_maths_Param_h
#define eckit_maths_Param_h

#include "Expr.h"

//--------------------------------------------------------------------------------------------

namespace eckit {
namespace maths {


class Param : public Expr {

    string name_;

    //virtual ExpPtr resolve(Scope & ctx);


public: //  methods

    static std::string class_name() { return "Param"; }
    static std::string sig() { return "_"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == Param::sig(); }

    Param(const string& name);

    virtual ~Param();

    virtual ExpPtr optimise() { return shared_from_this(); }

    virtual ValPtr evaluate( Scope& ctx );

    virtual ExpPtr clone();

    virtual std::string type_name() const { return Param::class_name(); }
    virtual std::string signature() const { return Param::sig(); }
    virtual std::string ret_signature() const { return Param::sig(); }

    virtual void print( std::ostream& o ) const;

};

ExpPtr parameter(const string& name);

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
