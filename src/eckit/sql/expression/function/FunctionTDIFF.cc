/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionTDIFF.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/types/DateTime.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionTDIFF> tdiffFunctionBuilder("tdiff");

FunctionTDIFF::FunctionTDIFF(const std::string& name, const expression::Expressions& args) :
    FunctionIntegerExpression(name, args) {}

FunctionTDIFF::FunctionTDIFF(const FunctionTDIFF& other) :
    FunctionIntegerExpression(other.name_, other.args_) {}

std::shared_ptr<SQLExpression> FunctionTDIFF::clone() const {
    return std::make_shared<FunctionTDIFF>(*this);
}

FunctionTDIFF::~FunctionTDIFF() {}

double FunctionTDIFF::eval(bool& missing) const {
    int indate = (int)args_[0]->eval(missing);
    int intime = (int)args_[1]->eval(missing);
    int andate = (int)args_[2]->eval(missing);
    int antime = (int)args_[3]->eval(missing);

    if (missing)
        return 0;

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

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
