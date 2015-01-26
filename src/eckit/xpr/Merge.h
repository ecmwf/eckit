/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Merge.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Merge_h
#define eckit_xpr_Merge_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a Merge combination of vectors
class Merge : public Function {

public: // methods

    static std::string nodeName() { return "Merge"; }

    Merge( ExpPtr l0, ExpPtr l1 );

    Merge( Stream& s );

    struct Register { Register(); };

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    Merge( args_t& a );

    virtual std::string typeName() const { return nodeName(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

    /// Adds support for count()
    virtual bool countable() const;
    virtual size_t count() const;

protected:

    virtual void encode(Stream& s) const;

    static ExpPtr compute( Scope& ctx, const args_t& p );

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Merge> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr merge( ExpPtr l0, ExpPtr l1 );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
