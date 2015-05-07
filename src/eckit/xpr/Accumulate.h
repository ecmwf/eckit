/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file Accumulate.h
/// @author Florian Rathgeber
/// @date May 2015

#ifndef eckit_xpr_Accumulate_h
#define eckit_xpr_Accumulate_h

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Reduces a vector to a Real
template < typename T >
class Accumulate : public Function {

public: // methods

    static const char * nodeName();

    Accumulate( ExpPtr v );

    Accumulate( Accumulate&& ) = default;

    Accumulate( Stream& s );

    Accumulate& operator=(Accumulate&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec();

private:

    Accumulate( args_t& a );

    virtual std::string factoryName() const;
    virtual const char * typeName() const;

    virtual ExpPtr cloneWith(args_t& a) const
    {
        return ExpPtr( new Accumulate<T>(a) );
    }

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Accumulate> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr sum    ( ExpPtr v );
ExpPtr product( ExpPtr v );
ExpPtr mean   ( ExpPtr v );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
