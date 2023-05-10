/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionNE.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/type/SQLType.h"
#include "eckit/utils/StringTools.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionNE> neFunctionBuilder("<>");

using namespace eckit::sql::type;

const type::SQLType* FunctionNE::type() const {
    return &type::SQLType::lookup("double");
}

FunctionNE::FunctionNE(const FunctionNE& other) :
    FunctionExpression(other.name_, other.args_), tmp_(other.tmp_) {}

FunctionNE::FunctionNE(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), tmp_(0) {}

std::shared_ptr<SQLExpression> FunctionNE::clone() const {
    return std::make_shared<FunctionNE>(*this);
}

FunctionNE::~FunctionNE() {}

bool FunctionNE::equal(const SQLExpression& l, const SQLExpression& r, bool& missing) {
    if (l.type()->getKind() == SQLType::stringType) {

        std::string v1(l.evalAsString(missing));
        std::string v2(r.evalAsString(missing));

        if (missing) {
            return false;
        }

        v1 = StringTools::trim(v1, "\t\n\v\f\r ");
        v2 = StringTools::trim(v2, "\t\n\v\f\r ");

        return (v1 != v2);
    }

    return l.eval(missing) != r.eval(missing);
}

double FunctionNE::eval(bool& missing) const {
    return equal(*args_[0], *args_[1], missing);
}

}  // namespace eckit::sql::expression::function
