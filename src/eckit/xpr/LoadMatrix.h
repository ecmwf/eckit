/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file LoadMatrix.h
/// @author Florian Rathgeber
/// @date May 2015

#ifndef eckit_xpr_LoadMatrix_h
#define eckit_xpr_LoadMatrix_h

#include "eckit/xpr/Value.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

/// Loads a Matrix from a data source
class LoadMatrix : public Value {

public: // methods

    static const char * nodeName() { return "xpr::LoadMatrix"; }

    static std::string sig() { return "m"; }

    LoadMatrix( size_t rows, size_t cols );

    LoadMatrix( LoadMatrix&& ) = default;

    LoadMatrix( Stream& s );

    LoadMatrix& operator=(LoadMatrix&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // virtual methods

    virtual std::string factoryName() const { return "xpr::loadMatrix"; }
    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;
    virtual void asJSON( JSON& s ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

protected: // virtual methods

    // From Streamable
    virtual void encode( Stream& s ) const;

protected: // members

    size_t rows_;
    size_t cols_;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<LoadMatrix> reanimator_;
};

//-----------------------------------------------------------------------------

ExpPtr loadMatrix( size_t rows, size_t cols );

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
