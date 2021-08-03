/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionTHIN.h
/// (C) ECMWF July 2010

#include "eckit/sql/expression/function/FunctionTHIN.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionTHIN> thinFunctionBuilder("thin");

FunctionTHIN::FunctionTHIN(const std::string& name, const expression::Expressions& args) :
    FunctionIntegerExpression(name, args), count_(0) {}

FunctionTHIN::FunctionTHIN(const FunctionTHIN& other) :
    FunctionIntegerExpression(other.name_, other.args_), count_(other.count_) {}

FunctionTHIN::~FunctionTHIN() {}

std::shared_ptr<SQLExpression> FunctionTHIN::clone() const {
    return std::make_shared<FunctionTHIN>(*this);
}

const eckit::sql::type::SQLType* FunctionTHIN::type() const {
    return &eckit::sql::type::SQLType::lookup("integer");
}

void FunctionTHIN::print(std::ostream& s) const {
    s << "THIN()";
}

double FunctionTHIN::eval(bool& missing) const {
    int every_nth = (int)args_[0]->eval(missing);
    if ((*count_) % every_nth == 0)
        return 1.0;
    else
        return 0.0;
}

void FunctionTHIN::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    count_ = &sql.count_;
}

void FunctionTHIN::cleanup(SQLSelect& sql) {}

bool FunctionTHIN::isConstant() const {
    return false;
}

std::shared_ptr<SQLExpression> FunctionTHIN::simplify(bool&) {
    return 0;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
