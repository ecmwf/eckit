/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Value.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Value::Value()
{
}

Value::Value(const args_t &args) : Expression(args)
{
}

Value::Value(args_t &args, Swap ignored) : Expression(args, ignored)
{
}

Value::Value(Stream &s) : Expression(s) {}

Value::~Value()
{
}

ExpPtr Value::evaluate( Scope &ctx ) const
{
    return self();
}

//--------------------------------------------------------------------------------------------

ClassSpec Value::classSpec_ = {
    &Expression::classSpec(),
    Value::nodeName(),
};

} // namespace xpr
} // namespace eckit
