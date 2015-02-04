/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Take.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Take_h
#define eckit_xpr_Take_h

#include "eckit/xpr/Function.h"
#include "eckit/xpr/Undef.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Takes an element from the list

class Take : public Function {

public: // methods

    static const char * nodeName() { return "Take"; }

    Take( ExpPtr e = undef(), ExpPtr l = undef()  );

    Take( Take&& ) = default;

    Take( Stream& s );

    Take& operator=(Take&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    Take( args_t& a );

    virtual std::string factoryName() const { return "xpr::take"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Take> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr take( ExpPtr e, ExpPtr l );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
