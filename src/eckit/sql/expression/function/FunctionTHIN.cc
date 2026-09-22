// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// \file FunctionTHIN.h
/// (C) ECMWF July 2010

#include "eckit/sql/expression/function/FunctionTHIN.h"

#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionTHIN> thinFunctionBuilder("thin");

FunctionTHIN::FunctionTHIN(const std::string& name, const expression::Expressions& args) :
    FunctionIntegerExpression(name, args), count_{nullptr} {}

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
    if ((*count_) % every_nth == 0) {
        return 1.0;
    }
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
    return nullptr;
}

}  // namespace eckit::sql::expression::function
