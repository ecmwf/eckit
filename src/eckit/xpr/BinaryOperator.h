/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file BinaryOperator.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_BinaryFunc_h
#define eckit_xpr_BinaryFunc_h

#include <functional>

#include "eckit/xpr/Function.h"
#include "eckit/xpr/Undef.h"


namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Binary operator
///
template <class T>
class BinaryOperator : public Function  {
public:

    static const char * nodeName();

    BinaryOperator(ExpPtr a, ExpPtr b);

    BinaryOperator(BinaryOperator&&) = default;

    BinaryOperator& operator=(BinaryOperator&&) = default;

    BinaryOperator(Stream& s);

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec();

private: // methods

    BinaryOperator( args_t& a );

    virtual std::string factoryName() const;
    virtual const char * typeName() const;

    virtual ExpPtr cloneWith(args_t& a) const
    {
        return ExpPtr( new BinaryOperator<T>(a) );
    }

private: // static members

    static  Reanimator< BinaryOperator > reanimator_;
};

//--------------------------------------------------------------------------------------------

typedef std::multiplies<real_t>  Prod;
typedef std::divides<real_t>     Div;
typedef std::plus<real_t>        Add;
typedef std::minus<real_t>       Sub;
typedef std::modulus<integer_t>  Mod;

struct Min;
struct Max;

//--------------------------------------------------------------------------------------------

// version with stand alone functions

ExpPtr prod( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr div ( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr add ( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr sub ( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr mod ( ExpPtr l = undef(), ExpPtr r = undef() );

ExpPtr min ( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr max ( ExpPtr l = undef(), ExpPtr r = undef() );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
