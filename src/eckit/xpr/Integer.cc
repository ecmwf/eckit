/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/JSON.h"

#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Scope.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Integer::Integer( const integer_t& v ) : v_(v)
{
}

Integer::Integer(Stream &s) : Value(s), v_(0)
{
    s >> v_;
}

void Integer::encode(Stream &s) const
{
    Value::encode(s);
    s << v_;
}

bool Integer::is(const ExpPtr &e)
{
    return dynamic_cast<Integer*>(e.get()) != 0;
}

void Integer::print(std::ostream&o) const
{
    o << nodeName() << "(" << v_ << ")";
}

Integer::Integer(ExpPtr e) : v_(0)
{
   v_ = Integer::extract( e->eval(false) );
}

ExpPtr Integer::cloneWith(args_t& a) const {
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ClassSpec Integer::classSpec_ = {
    &Value::classSpec(),
    Integer::nodeName(),
};

Reanimator< Integer > Integer::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr integer(const integer_t &s)
{
    return ExpPtr( new Integer(s) );
}

void Integer::asCode(std::ostream&o) const
{
    o << factoryName() << "(" << v_ << ")";
}

void Integer::asJSON(JSON& s) const
{
    s << v_;
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
