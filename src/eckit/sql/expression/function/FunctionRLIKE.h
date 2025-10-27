/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \File FunctionRLIKE.h
/// Piotr Kuchta - ECMWF Sep 2014

#ifndef FunctionRLIKE_H
#define FunctionRLIKE_H

#include <memory>

#include "eckit/sql/expression/function/FunctionExpression.h"
#include "eckit/utils/Regex.h"

namespace eckit::sql::expression::function {

class FunctionRLIKE : public FunctionExpression {
public:

    FunctionRLIKE(const std::string&, const expression::Expressions&);
    FunctionRLIKE(const FunctionRLIKE&);
    ~FunctionRLIKE();

    bool match(const SQLExpression& l, const SQLExpression& r, bool& missing) const;
    static void trimStringInDouble(char*& p, size_t& len);

    std::shared_ptr<SQLExpression> clone() const override;
    void prepare(SQLSelect&) override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionRLIKE& operator=(const FunctionRLIKE&);

    std::unique_ptr<eckit::Regex> re_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionRLIKE& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
