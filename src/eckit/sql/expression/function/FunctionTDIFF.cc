/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/DateTime.h"
#include "eckit/sql/expression/function/FunctionTDIFF.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

FunctionTDIFF::FunctionTDIFF(const std::string& name, const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionTDIFF::FunctionTDIFF(const FunctionTDIFF& other)
: FunctionExpression(other.name_, other.args_)
{}

std::shared_ptr<SQLExpression> FunctionTDIFF::clone() const { return std::make_shared<FunctionTDIFF>(*this);  }

FunctionTDIFF::~FunctionTDIFF() {}

double FunctionTDIFF::eval(bool& missing) const
{
    int indate = (int) args_[0]->eval(missing);
    int intime = (int) args_[1]->eval(missing);
    int andate = (int) args_[2]->eval(missing);
    int antime = (int) args_[3]->eval(missing);

    eckit::Date d1(indate);
    eckit::Date d2(andate);

    eckit::Time t1(intime/10000, (intime % 10000) / 100, intime % 100);
    eckit::Time t2(antime/10000, (antime % 10000) / 100, antime % 100);

    eckit::DateTime dt1(d1, t1);
    eckit::DateTime dt2(d2, t2);

    return d1 - d2;
}

const eckit::sql::type::SQLType* FunctionTDIFF::type() const { return &eckit::sql::type::SQLType::lookup("integer"); }

void FunctionTDIFF::output(std::ostream& s) const
{
	bool missing;
    s << static_cast<long long int>(eval(missing));
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

