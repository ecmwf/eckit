/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionRLIKE.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/type/SQLType.h"
#include "eckit/utils/Regex.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionRLIKE> rlikeFunctionBuilder("rlike");
static FunctionBuilder<FunctionRLIKE> likeFunctionBuilder("like");

using namespace eckit::sql::type;

const type::SQLType* FunctionRLIKE::type() const {
    return &type::SQLType::lookup("double");
}

FunctionRLIKE::FunctionRLIKE(const FunctionRLIKE& other) :
    FunctionExpression(other.name_, other.args_), re_(new eckit::Regex(*other.re_)) {}

FunctionRLIKE::FunctionRLIKE(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), re_() {}

std::shared_ptr<SQLExpression> FunctionRLIKE::clone() const {
    return std::make_shared<FunctionRLIKE>(*this);
}

FunctionRLIKE::~FunctionRLIKE() {}

void FunctionRLIKE::trimStringInDouble(char*& p, size_t& len) {
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


void FunctionRLIKE::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);

    SQLExpression &l(*args_[0]), &r(*args_[1]);

    if (l.type()->getKind() != SQLType::stringType || r.type()->getKind() != SQLType::stringType)
        throw eckit::UserError("Arguments of RLIKE must be of string type");

    bool missing(false);
    double v2(r.eval(missing));
    char* p2 = reinterpret_cast<char*>(&v2);
    size_t len2(sizeof(double));

    trimStringInDouble(p2, len2);

    std::string re(p2, len2);
    // eckit::Log::info() << "FunctionRLIKE::prepare: regex: '" << re << "'" << std::endl;
    re_.reset(new eckit::Regex(re));
}

bool FunctionRLIKE::match(const SQLExpression& l, const SQLExpression& r, bool& missing) const {
    double v1 = l.eval(missing);
    if (missing)
        return false;

    char* p1(reinterpret_cast<char*>(&v1));
    size_t len1(sizeof(double));

    trimStringInDouble(p1, len1);
    std::string s1(p1, len1);

    bool ret = re_->match(s1);
    // eckit::Log::info() << "FunctionRLIKE::match '" << s1 << "' => " << ret << std::endl;
    return ret;
}

double FunctionRLIKE::eval(bool& missing) const {
    return match(*args_[0], *args_[1], missing);
}


}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
