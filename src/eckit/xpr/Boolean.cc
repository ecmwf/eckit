/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/JSON.h"

#include "eckit/xpr/Boolean.h"
#include "eckit/xpr/Scope.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Boolean::Boolean( const Boolean::value_t& v ) : v_(v)
{
}

Boolean::Boolean(Stream &s) : Value(s), v_(0)
{
    s >> v_;
}

void Boolean::encode(Stream &s) const
{
    Value::encode(s);
    s << v_;
}

void Boolean::print(std::ostream&o) const
{
    o << nodeName() << "(" << (v_? "true" : "false") << ")";
}

void Boolean::asCode(std::ostream&o) const
{
    o << factoryName() << "(" << (v_? "true" : "false") << ")";
}

void Boolean::asJSON(JSON& s) const
{
    s << v_;
}

Boolean::Boolean(const ExpPtr& e) : v_(0)
{
   Scope dummy("Boolean::Boolean");
   v_ = Boolean::extract( dummy, e->eval(dummy) );
}

ExpPtr Boolean::cloneWith(args_t& a) const
{
    NOTIMP; // Should not be called
}

//--------------------------------------------------------------------------------------------

ClassSpec Boolean::classSpec_ = {
    &Value::classSpec(),
    Boolean::nodeName(),
};

Reanimator< Boolean > Boolean::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr boolean(const Boolean::value_t &s)
{
    return ExpPtr( new Boolean(s) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
