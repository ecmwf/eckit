/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionEQ.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/type/SQLType.h"
#include "eckit/utils/StringTools.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionEQ> eqFunctionBuilder("=");

using namespace eckit::sql::type;

const type::SQLType* FunctionEQ::type() const {
    return &type::SQLType::lookup("double");
}

FunctionEQ::FunctionEQ(const FunctionEQ& other) :
    FunctionExpression(other.name_, other.args_), tmp_(other.tmp_) {}

FunctionEQ::FunctionEQ(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), tmp_(0) {}

std::shared_ptr<SQLExpression> FunctionEQ::clone() const {
    return std::make_shared<FunctionEQ>(*this);
}

FunctionEQ::~FunctionEQ() {}


void FunctionEQ::trimStringInDouble(char*& p, size_t& len) {
    // n.b. Duplicated into odb::WriterDispatchingIterator
    //      TODO: Put somewhere better.
    len = 0;
    for (; len < sizeof(double) && isprint(p[len]); ++len)
        ;
    for (; len > 0 && isspace(p[len - 1]); --len)
        ;
    size_t plen = len;
    for (char* pp = p; isspace(*p) && p < pp + plen;) {
        ++p;
        --len;
    }
}

bool FunctionEQ::equal(const SQLExpression& l, const SQLExpression& r, bool& missing) {
    if (l.type()->getKind() == SQLType::stringType) {

        std::string v1(l.evalAsString(missing));
        std::string v2(r.evalAsString(missing));

        if (missing)
            return false;

        v1 = StringTools::trim(v1, "\t\n\v\f\r ");
        v2 = StringTools::trim(v2, "\t\n\v\f\r ");

        return (v1 == v2);
    }

    return l.eval(missing) == r.eval(missing);
}

double FunctionEQ::eval(bool& missing) const {
    return equal(*args_[0], *args_[1], missing);
}

std::shared_ptr<SQLExpression> FunctionEQ::simplify(bool& changed) {
    std::shared_ptr<SQLExpression> x = FunctionExpression::simplify(changed);
    if (x)
        return x;

    ColumnExpression* a = dynamic_cast<ColumnExpression*>(args_[0].get());
    ColumnExpression* b = dynamic_cast<ColumnExpression*>(args_[1].get());

    if (a && b) {
        return FunctionFactory::instance().build("join", args_[0], args_[1]);
    }

    //
    if (args_[0]->isConstant() && !args_[1]->isConstant())
        std::swap(args_[0], args_[1]);

    return 0;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
