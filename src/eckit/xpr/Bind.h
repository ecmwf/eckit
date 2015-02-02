/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Bind.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Bind_h
#define eckit_xpr_Bind_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a Bind combination of vectors
class Bind : public Function {

public: // methods

    static std::string nodeName() { return "Bind"; }

    Bind( size_t i, ExpPtr f, ExpPtr e );
    Bind( ExpPtr i, ExpPtr f, ExpPtr e );

    Bind( Bind&& ) = default;

    Bind( Stream& s );

    Bind& operator=(Bind&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

protected: // virtual methods

    // From Streamable
    virtual void encode(Stream& s) const;

    virtual void asCode( std::ostream& ) const;

private: // methods

    Bind(args_t& a);

    virtual std::string factoryName() const { return "xpr::bind"; }
    virtual std::string typeName() const;

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Bind> reanimator_;
};

//--------------------------------------------------------------------------------------------

template < int i >
ExpPtr bind( ExpPtr f, ExpPtr p )
{
    return ExpPtr( new Bind( i, f, p ) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
