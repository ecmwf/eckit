/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/xpr/Math.h"
#include "eckit/xpr/Value.h"
#include "eckit/xpr/Scope.h"
#include "eckit/xpr/Undef.h"
#include "eckit/xpr/Param.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/BinaryPredicate.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/Expression.h"
#include "eckit/xpr/Map.h"
#include "eckit/xpr/Filter.h"
#include "eckit/xpr/Reduce.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/ZipWith.h"
#include "eckit/xpr/Bind.h"
#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/Param.h"
#include "eckit/xpr/Call.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Math::Math(ExpPtr e):
    expr_(e)
{
}

Math::Math(double v):
    expr_(xpr::scalar(v))
{
}

Math::Math(const std::string& v):
    expr_(xpr::parameter(v))
{
}

Math::Math(const char* v):
    expr_(xpr::parameter(v))
{
}


Math::Math(bool v):
    expr_(xpr::boolean(v))
{
}

Math::~Math()
{
}

Math Math::operator()() const
{
    ExpPtr e = expr_->eval();
    return Math(e);
}

Math Math::operator()(const Math& a) const
{
    ExpPtr e = expr_->eval(a);
    return Math(e);
}

Math Math::operator()(const Math& a, const Math& b) const
{
    ExpPtr e = expr_->eval(a, b);
    return Math(e);
}

void Math::print(std::ostream& os) const
{
    os << *expr_;
}

Math::operator ExpPtr() const
{
    return expr_;
}

Math Math::operator >(const Math& other) const
{
    return xpr::greater(*this, other);
}

Math Math::operator ==(const Math& other) const
{
    return xpr::equal(*this, other);
}

Math Math::operator *(const Math& other) const
{
    return xpr::prod(*this, other);
}

Math Math::operator -(const Math& other) const
{
    return xpr::sub(*this, other);
}

Math Math::operator +(const Math& other) const
{
    return xpr::add(*this, other);
}

Math Math::optimise() const
{
    return Math(expr_->optimise(true));
}

ExpPtr Math::expr() const
{
    return expr_;
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
