// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cmath>

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionSTDEV.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionSTDEV> stdevFunctionBuilder("stdev");
static FunctionBuilder<FunctionSTDEV> stddevFunctionBuilder("stddev");
static FunctionBuilder<FunctionSTDEV> stdevpFunctionBuilder("stdevp");

FunctionSTDEV::FunctionSTDEV(const std::string& name, const expression::Expressions& args) : FunctionVAR(name, args) {}

FunctionSTDEV::FunctionSTDEV(const FunctionSTDEV& other) : FunctionVAR(static_cast<const FunctionVAR&>(other)) {}

FunctionSTDEV::~FunctionSTDEV() {}

std::shared_ptr<SQLExpression> FunctionSTDEV::clone() const {
    return std::make_shared<FunctionSTDEV>(*this);
}

const type::SQLType* FunctionSTDEV::type() const {
    return &type::SQLType::lookup("double");
}

double FunctionSTDEV::eval(bool& missing) const {
    double x = FunctionVAR::eval(missing);
    if (x < 0) {
        x = 0;
    }
    return sqrt(x);
}

}  // namespace eckit::sql::expression::function
