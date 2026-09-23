// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionTDIFF.h"

#include "eckit/sql/SQLOutput.h"
#include "eckit/types/DateTime.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionTDIFF> tdiffFunctionBuilder("tdiff");

FunctionTDIFF::FunctionTDIFF(const std::string& name, const expression::Expressions& args) :
    FunctionIntegerExpression(name, args) {}

FunctionTDIFF::FunctionTDIFF(const FunctionTDIFF& other) : FunctionIntegerExpression(other.name_, other.args_) {}

std::shared_ptr<SQLExpression> FunctionTDIFF::clone() const {
    return std::make_shared<FunctionTDIFF>(*this);
}

FunctionTDIFF::~FunctionTDIFF() {}

double FunctionTDIFF::eval(bool& missing) const {
    int indate = (int)args_[0]->eval(missing);
    int intime = (int)args_[1]->eval(missing);
    int andate = (int)args_[2]->eval(missing);
    int antime = (int)args_[3]->eval(missing);

    if (missing) {
        return 0;
    }

    // Check for invalid values

    try {
        eckit::Date d1(indate);
        eckit::Date d2(andate);

        eckit::Time t1(intime / 10000, (intime % 10000) / 100, intime % 100);
        eckit::Time t2(antime / 10000, (antime % 10000) / 100, antime % 100);

        eckit::DateTime dt1(d1, t1);
        eckit::DateTime dt2(d2, t2);

        return dt1 - dt2;
    }
    catch (BadValue& e) {
        missing = true;
        return 0;
    }
}

const eckit::sql::type::SQLType* FunctionTDIFF::type() const {
    return &eckit::sql::type::SQLType::lookup("integer");
}

}  // namespace eckit::sql::expression::function
