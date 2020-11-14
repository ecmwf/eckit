/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Dec 2003

#ifndef eckit_sql_FunctionExpression_H
#define eckit_sql_FunctionExpression_H

#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

//----------------------------------------------------------------------------------------------------------------------

class FunctionExpression : public SQLExpression {
public:
    FunctionExpression(const std::string&, const expression::Expressions&);
    FunctionExpression(const FunctionExpression&);
    ~FunctionExpression();

    virtual void print(std::ostream& s) const override;
    virtual void preprepare(SQLSelect& sql) override;
    virtual void prepare(SQLSelect& sql) override;
    virtual void updateType(SQLSelect& sql) override;
    virtual void cleanup(SQLSelect& sql) override;
    virtual bool isConstant() const override;
    virtual std::shared_ptr<SQLExpression> simplify(bool&) override;

    // virtual double eval() const override;
    bool isAggregate() const override;
    void partialResult() override;

    virtual const type::SQLType* type() const override;
    virtual std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;

    // For SQLSelectFactory (maybe it should just friend SQLSelectFactory).
    expression::Expressions& args() { return args_; }

    static const char* help() { return ""; }

protected:
    std::string name_;
    expression::Expressions args_;
    // void print(std::ostream&) const override;

    // -- Overridden methods

    void tables(std::set<const SQLTable*>&) override;

private:
    FunctionExpression& operator=(const FunctionExpression&);

    // For use inside reshift()
    void shiftArgs(int minColumnShift);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
