/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionROWNUMBER.h
/// Piotr Kuchta - (C) ECMWF July 2009

#include "eckit/sql/expression/function/FunctionROWNUMBER.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionROWNUMBER> rownumberFunctionBuilder("rownumber");

FunctionROWNUMBER::FunctionROWNUMBER(const std::string& name, const expression::Expressions& args) :
    FunctionIntegerExpression(name, args), count_(0) {}


FunctionROWNUMBER::FunctionROWNUMBER(const FunctionROWNUMBER& other) :
    FunctionIntegerExpression(other.name_, other.args_), count_(other.count_) {}

std::shared_ptr<SQLExpression> FunctionROWNUMBER::clone() const {
    return std::make_shared<FunctionROWNUMBER>(*this);
}

FunctionROWNUMBER::~FunctionROWNUMBER() {}

void FunctionROWNUMBER::print(std::ostream& s) const {
    s << "rownumber()";
}

double FunctionROWNUMBER::eval(bool& missing) const {
    return *count_;
}

void FunctionROWNUMBER::prepare(SQLSelect& sql) {
    count_ = &sql.total_;
}

void FunctionROWNUMBER::cleanup(SQLSelect& sql) {}

bool FunctionROWNUMBER::isConstant() const {
    return false;
}

std::shared_ptr<SQLExpression> FunctionROWNUMBER::simplify(bool&) {
    return 0;
}

void FunctionROWNUMBER::partialResult() { /*NOTIMP;*/
}

const eckit::sql::type::SQLType* FunctionROWNUMBER::type() const {
    return &eckit::sql::type::SQLType::lookup("integer");
}


}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
