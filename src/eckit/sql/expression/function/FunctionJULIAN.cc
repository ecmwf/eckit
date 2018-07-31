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
#include "odb_api/FunctionJULIAN.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

FunctionJULIAN::FunctionJULIAN(const std::string& name,const expression::Expressions& args)
: FunctionExpression(name,args)
{}

FunctionJULIAN::FunctionJULIAN(const FunctionJULIAN& other)
: FunctionExpression(other.name_, other.args_)
{}


SQLExpression* FunctionJULIAN::clone() const { return new FunctionJULIAN(*this); }


FunctionJULIAN::~FunctionJULIAN() {}

const type::SQLType* FunctionJULIAN::type() const { return &type::SQLType::lookup("real"); }

double FunctionJULIAN::eval(bool& missing) const
{
    int indate = (int) args_[0]->eval(missing);
    int intime = (int) args_[1]->eval(missing);
	// TODO: shold we return MISSING_VALUE_INT in case missing == true here?
    int year_target = indate/10000;
    int month_target = (indate%10000)/100;
    int day_target = indate%100;
    int hour_target = intime/10000;
    int min_target = (intime%10000)/100;
    int sec_target = intime%100;

    utils::DateTime d1(year_target, month_target, day_target,
                   hour_target, min_target, sec_target);

    return d1.dateToJulian();
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

