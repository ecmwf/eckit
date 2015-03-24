/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Count.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Count_h
#define eckit_xpr_Count_h

#include "eckit/xpr/Function.h"
#include "eckit/xpr/Undef.h"


namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a Count combination of vectors
class Count : public Function {

public: // methods

    static const char * nodeName() { return "xpr::Count"; }

    Count( ExpPtr e = undef() );

    Count ( Count&& ) = default;

    Count ( Stream& s );

    Count& operator=(Count&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    Count( args_t& a );

    virtual std::string factoryName() const { return "xpr::count"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

    virtual ExpPtr optimise(size_t depth) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Count> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr count( ExpPtr e );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
