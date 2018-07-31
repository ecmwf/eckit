/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/DateTime.h"
#include "odb_api/FunctionTDIFF.h"
#include "odb_api/MDI.h"

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

SQLExpression* FunctionTDIFF::clone() const { return new FunctionTDIFF(*this);  }

FunctionTDIFF::~FunctionTDIFF() {}

double FunctionTDIFF::eval(bool& missing) const
{
    int indate = (int) args_[0]->eval(missing);
    int intime = (int) args_[1]->eval(missing);
    int andate = (int) args_[2]->eval(missing);
    int antime = (int) args_[3]->eval(missing);

    int year_target = indate/10000;
    int month_target = (indate%10000)/100;
    int day_target = indate%100;
    int hour_target = intime/10000;
    int min_target = (intime%10000)/100;
    int sec_target = intime%100;

    int year_anal = andate/10000;
    int month_anal = (andate%10000)/100;
    int day_anal = andate%100;
    int hour_anal = antime/10000;
    int min_anal = (antime%10000)/100;
    int sec_anal = antime%100;

    int seconds = eckit::MDI::realMDI();

    utils::DateTime d1(year_target, month_target, day_target,
                   hour_target, min_target, sec_target);
    utils::DateTime d2(year_anal, month_anal, day_anal, 
                   hour_anal, min_anal, sec_anal);

    seconds = d1.secondsDateMinusDate(d2);

    return seconds;
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

