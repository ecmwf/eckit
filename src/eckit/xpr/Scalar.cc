/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Scope.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Scalar::Scalar( const scalar_t& v ) : v_(v)
{
}

Scalar::Scalar(Stream &s) : Value(s), v_(0)
{
    s >> v_;
}

void Scalar::encode(Stream &s) const
{
    Value::encode(s);
    s << v_;
}

bool Scalar::is(const ExpPtr &e)
{
    return dynamic_cast<Scalar*>(e.get()) != 0;
}

void Scalar::print(std::ostream&o) const
{
    o << nodeName() << "(" << v_ << ")";
}

Scalar::Scalar(ExpPtr e) : v_(0)
{
   Scope dummy("Scalar::Scalar");
   v_ = Scalar::extract(dummy, e->eval(dummy) );
}

ExpPtr Scalar::cloneWith(args_t& a) const {
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ClassSpec Scalar::classSpec_ = {
    &Value::classSpec(),
    Scalar::nodeName().c_str(),
};

Reanimator< Scalar > Scalar::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr scalar(const scalar_t &s)
{
    return ExpPtr( new Scalar(s) );
}

void Scalar::asCode(std::ostream&o) const
{
    o << "xpr::scalar(" << v_ << ")";
}


//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
