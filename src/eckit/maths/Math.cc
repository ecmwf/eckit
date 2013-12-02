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

#include "eckit/maths/Math.h"
#include "eckit/maths/Value.h"
#include "eckit/maths/Scope.h"
#include "eckit/maths/Undef.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/ParamDef.h"
#include "eckit/maths/BinaryOperator.h"
#include "eckit/maths/BinaryPredicate.h"
#include "eckit/maths/Count.h"
#include "eckit/maths/Expression.h"
#include "eckit/maths/Map.h"
#include "eckit/maths/Filter.h"
#include "eckit/maths/Reduce.h"
#include "eckit/maths/List.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/UnaryOperator.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/ZipWith.h"
#include "eckit/maths/Bind.h"
#include "eckit/maths/IfElse.h"
#include "eckit/maths/Lambda.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/Call.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Math::Math(ExpPtr e):
    expr_(e)
{
}

Math::Math(double v):
    expr_(maths::scalar(v))
{
}

Math::Math(const std::string& v):
    expr_(maths::parameter(v))
{
}

Math::Math(const char* v):
    expr_(maths::parameter(v))
{
}


Math::Math(bool v):
    expr_(maths::boolean(v))
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
    return maths::greater(*this, other);
}

Math Math::operator ==(const Math& other) const
{
    return maths::equal(*this, other);
}

Math Math::operator *(const Math& other) const
{
    return maths::prod(*this, other);
}

Math Math::operator -(const Math& other) const
{
    return maths::sub(*this, other);
}

Math Math::operator +(const Math& other) const
{
    return maths::add(*this, other);
}

Math Math::optimise() const
{
    return Math(expr_->optimise(true));
}
//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
