/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Map.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_FMap_h
#define eckit_xpr_FMap_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a Map combination of vectors
class Map : public Function {

public: // methods

    static std::string nodeName() { return "Map"; }

    Map( ExpPtr f, ExpPtr l );

    Map(Map&&) = default;

    Map(Stream& s);

    Map& operator=(Map&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

protected: // virtual methods

    // From Streamable
    virtual void encode(Stream& s) const;

private: // methods

    Map( args_t& a );

    virtual std::string typeName() const { return nodeName(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

    /// Adds support for count()
    virtual bool countable() const;
    virtual size_t count() const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Map> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr map( ExpPtr f, ExpPtr l );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
