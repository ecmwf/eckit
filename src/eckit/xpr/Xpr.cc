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

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/BinaryPredicate.h"
#include "eckit/xpr/Bind.h"
#include "eckit/xpr/Call.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/Filter.h"
#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Map.h"
#include "eckit/xpr/Param.h"
#include "eckit/xpr/Param.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/Reduce.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Scope.h"
#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Undef.h"
#include "eckit/xpr/Value.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/Xpr.h"
#include "eckit/xpr/ZipWith.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Xpr::Xpr(ExpPtr e):
    expr_(e)
{
}

Xpr::Xpr(double v):
    expr_(xpr::real(v))
{
}

Xpr::Xpr(int v):
    expr_(xpr::real(v))
{
}

Xpr::Xpr(std::initializer_list<double> v):
    expr_(xpr::vector(v))
{
}

Xpr::Xpr(const std::string& v):
    expr_(xpr::parameter(v))
{
}

Xpr::Xpr(const char* v):
    expr_(xpr::parameter(v))
{
}


Xpr::Xpr(bool v):
    expr_(xpr::boolean(v))
{
}

Xpr::~Xpr()
{
}

Xpr Xpr::operator()() const
{
    ExpPtr e = expr_->eval();
    return Xpr(e);
}

Xpr Xpr::operator()(const Xpr& a) const
{
    ExpPtr e = expr_->eval(a);
    return Xpr(e);
}

Xpr Xpr::operator()(const Xpr& a, const Xpr& b) const
{
    ExpPtr e = expr_->eval(a, b);
    return Xpr(e);
}

void Xpr::print(std::ostream& os) const
{
    os << *expr_;
}

Xpr::operator ExpPtr() const
{
    return expr_;
}

Xpr Xpr::operator -(const Xpr& other) const
{
    return xpr::sub(*this, other);
}

Xpr Xpr::operator +(const Xpr& other) const
{
    return xpr::add(*this, other);
}

Xpr Xpr::operator /(const Xpr& other) const
{
    return xpr::div(*this, other);
}

Xpr Xpr::operator *(const Xpr& other) const
{
    return xpr::prod(*this, other);
}

Xpr Xpr::operator ==(const Xpr& other) const
{
    return xpr::equal(*this, other);
}

Xpr Xpr::operator !=(const Xpr& other) const
{
    return xpr::not_equal(*this, other);
}

Xpr Xpr::operator >(const Xpr& other) const
{
    return xpr::greater(*this, other);
}

Xpr Xpr::operator <(const Xpr& other) const
{
    return xpr::less(*this, other);
}

Xpr Xpr::operator >=(const Xpr& other) const
{
    return xpr::greater_equal(*this, other);
}

Xpr Xpr::operator <=(const Xpr& other) const
{
    return xpr::less_equal(*this, other);
}

Xpr Xpr::optimise() const
{
    return Xpr(expr_->optimise(true));
}

ExpPtr Xpr::expr() const
{
    return expr_;
}

Xpr operator - (double a, const Xpr& b) { return Xpr(a) - b; }
Xpr operator - (int a,    const Xpr& b) { return Xpr(a) - b; }
Xpr operator + (double a, const Xpr& b) { return Xpr(a) + b; }
Xpr operator + (int a,    const Xpr& b) { return Xpr(a) + b; }
Xpr operator / (double a, const Xpr& b) { return Xpr(a) / b; }
Xpr operator / (int a,    const Xpr& b) { return Xpr(a) / b; }
Xpr operator * (double a, const Xpr& b) { return Xpr(a) * b; }
Xpr operator * (int a,    const Xpr& b) { return Xpr(a) * b; }
Xpr operator ==(double a, const Xpr& b) { return Xpr(a) == b; }
Xpr operator ==(int a,    const Xpr& b) { return Xpr(a) == b; }
Xpr operator !=(double a, const Xpr& b) { return Xpr(a) != b; }
Xpr operator !=(int a,    const Xpr& b) { return Xpr(a) != b; }
Xpr operator > (double a, const Xpr& b) { return Xpr(a) > b; }
Xpr operator > (int a,    const Xpr& b) { return Xpr(a) > b; }
Xpr operator < (double a, const Xpr& b) { return Xpr(a) < b; }
Xpr operator < (int a,    const Xpr& b) { return Xpr(a) < b; }
Xpr operator >=(double a, const Xpr& b) { return Xpr(a) >= b; }
Xpr operator >=(int a,    const Xpr& b) { return Xpr(a) >= b; }
Xpr operator <=(double a, const Xpr& b) { return Xpr(a) <= b; }
Xpr operator <=(int a,    const Xpr& b) { return Xpr(a) <= b; }

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
