/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <limits.h>
#include <cmath>

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionTIMESTAMP.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionTIMESTAMP> timestampFunctionBuilder("timestamp");

FunctionTIMESTAMP::FunctionTIMESTAMP(const std::string& name, const expression::Expressions& args) :
    FunctionIntegerExpression(name, args) {}

FunctionTIMESTAMP::FunctionTIMESTAMP(const FunctionTIMESTAMP& other) :
    FunctionIntegerExpression(other.name_, other.args_) {}

std::shared_ptr<SQLExpression> FunctionTIMESTAMP::clone() const {
    return std::make_shared<FunctionTIMESTAMP>(*this);
}

FunctionTIMESTAMP::~FunctionTIMESTAMP() {}

double FunctionTIMESTAMP::eval(bool& missing) const {
    double indate = args_[0]->eval(missing);
    double intime = args_[1]->eval(missing);
    // Merge "YYYYMMDD" and "HHMMSS" into "YYYYMMDDHHMMSS"
    double outstamp = 0;
    if (indate >= 0 && indate <= 99991231 && intime >= 0 && intime <= 240000) {
        long long int lldate = (long long int)indate;
        long long int lltime = (long long int)intime;
        long long int tstamp = lldate * 1000000ll + lltime;
        outstamp             = tstamp;
        outstamp             = trunc(outstamp);
    }
    else {
        missing = true;
    }

    return outstamp;
}

const eckit::sql::type::SQLType* FunctionTIMESTAMP::type() const {
    return &eckit::sql::type::SQLType::lookup("integer");
}


}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
