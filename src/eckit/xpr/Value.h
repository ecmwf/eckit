/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Value.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Value_h
#define eckit_xpr_Value_h

#include "eckit/xpr/Expression.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class Value : public Expression {

public: // methods

    static std::string nodeName() { return "Value"; }

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Value();

    /// Contructor taking a list of parameters
    Value( const args_t& args );
    Value( args_t& args, Swap ignored );

    Value(Value&&) = default;

    Value(Stream& s);

    virtual ~Value();

    Value& operator=(Value&&) = default;

    // Class members
    static const ClassSpec& classSpec() { return classSpec_; }

protected: // virtual methods

    // From Streamable
    virtual void encode(eckit::Stream& s) const;

private:

    virtual ExpPtr evaluate( Scope& ) const;

    static  ClassSpec classSpec_;
};

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
