/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionSTDEV.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionSTDEV> stdevFunctionBuilder("stdev");
static FunctionBuilder<FunctionSTDEV> stddevFunctionBuilder("stddev");
static FunctionBuilder<FunctionSTDEV> stdevpFunctionBuilder("stdevp");

FunctionSTDEV::FunctionSTDEV(const std::string& name, const expression::Expressions& args) :
    FunctionVAR(name, args) {}

FunctionSTDEV::FunctionSTDEV(const FunctionSTDEV& other) :
    FunctionVAR(static_cast<const FunctionVAR&>(other)) {}

FunctionSTDEV::~FunctionSTDEV() {}

std::shared_ptr<SQLExpression> FunctionSTDEV::clone() const {
    return std::make_shared<FunctionSTDEV>(*this);
}

const type::SQLType* FunctionSTDEV::type() const {
    return &type::SQLType::lookup("double");
}

double FunctionSTDEV::eval(bool& missing) const {
    double x = FunctionVAR::eval(missing);
    if (x < 0)
        x = 0;
    return sqrt(x);
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
