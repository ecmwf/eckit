/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionJULIAN.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/types/Date.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionJULIAN> julianFunctionBuilder("julian");
static FunctionBuilder<FunctionJULIAN> jdFunctionBuilder("jd");
static FunctionBuilder<FunctionJULIAN> julian_dateFunctionBuilder("julian_date");

FunctionJULIAN::FunctionJULIAN(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionJULIAN::FunctionJULIAN(const FunctionJULIAN& other) :
    FunctionExpression(other.name_, other.args_) {}


std::shared_ptr<SQLExpression> FunctionJULIAN::clone() const {
    return std::make_shared<FunctionJULIAN>(*this);
}


FunctionJULIAN::~FunctionJULIAN() {}

const type::SQLType* FunctionJULIAN::type() const {
    return &type::SQLType::lookup("real");
}

double FunctionJULIAN::eval(bool& missing) const {
    int indate = (int)args_[0]->eval(missing);
    // int intime = (int) args_[1]->eval(missing); // unused
    // TODO: shold we return MISSING_VALUE_INT in case missing == true here?

    eckit::Date date(indate);
    return date.julian();
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
