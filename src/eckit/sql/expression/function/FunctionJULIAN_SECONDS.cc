/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionJULIAN_SECONDS.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/types/Date.h"

#define trunc(x) ((x)-fmod((x), 1))

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionJULIAN_SECONDS> julian_secondsFunctionBuilder("julian_seconds");

FunctionJULIAN_SECONDS::FunctionJULIAN_SECONDS(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionJULIAN_SECONDS::FunctionJULIAN_SECONDS(const FunctionJULIAN_SECONDS& other) :
    FunctionExpression(other.name_, other.args_) {}

std::shared_ptr<SQLExpression> FunctionJULIAN_SECONDS::clone() const {
    return std::make_shared<FunctionJULIAN_SECONDS>(*this);
}

FunctionJULIAN_SECONDS::~FunctionJULIAN_SECONDS() {}

const type::SQLType* FunctionJULIAN_SECONDS::type() const {
    return &type::SQLType::lookup("real");
}

double FunctionJULIAN_SECONDS::eval(bool& missing) const {
    int indate = (int)args_[0]->eval(missing);
    int intime = (int)args_[1]->eval(missing);
    // TODO: shold we return MISSING_VALUE_INT in case missing == true here?

    int year  = indate / 10000;
    int month = (indate % 10000) / 100;
    int day   = indate % 100;
    int hour  = intime / 10000;
    int min   = (intime % 10000) / 100;
    int sec   = intime % 100;

    //  " Julianday * 24 * 60 * 60 + hh * 3600 + mm * 60 + ss ";

    return eckit::Date(year, month, day).julian() * 24 * 60 * 60 + hour * 3600 + min * 60 + sec;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
