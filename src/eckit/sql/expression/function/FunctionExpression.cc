// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

//----------------------------------------------------------------------------------------------------------------------

FunctionExpression::FunctionExpression(const std::string& name, const expression::Expressions& args) :
    name_(name), args_(args) {
    //  never use any logging here (Log::*)
    //	std::cout << "new FunctionExpression " << name << std::endl;
}

FunctionExpression::FunctionExpression(const FunctionExpression& other) : name_(other.name_), args_(other.args_) {}


const type::SQLType* FunctionExpression::type() const {
    return &type::SQLType::lookup("double");
}

std::shared_ptr<SQLExpression> FunctionExpression::reshift(int minColumnShift) const {
    std::shared_ptr<SQLExpression> shifted = clone();
    static_cast<FunctionExpression&>(*shifted).shiftArgs(minColumnShift);
    return shifted;
}

FunctionExpression::~FunctionExpression() {}

void FunctionExpression::preprepare(SQLSelect& sql) {
    for (expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j) {
        (*j)->preprepare(sql);
    }
}

void FunctionExpression::prepare(SQLSelect& sql) {
    for (expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j) {
        (*j)->prepare(sql);
    }
}

void FunctionExpression::updateType(SQLSelect& sql) {
    for (expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j) {
        (*j)->updateType(sql);
    }
}

void FunctionExpression::cleanup(SQLSelect& sql) {
    for (expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j) {
        (*j)->cleanup(sql);
    }
}

void FunctionExpression::partialResult() {
    for (expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j) {
        (*j)->partialResult();
    }
}


std::shared_ptr<SQLExpression> FunctionExpression::simplify(bool& changed) {
    for (std::shared_ptr<SQLExpression>& arg : args_) {

        std::shared_ptr<SQLExpression> simplifiedArg = arg->simplify(changed);

        if (simplifiedArg) {
            arg = simplifiedArg;
            std::cout << "SIMPLIFY " << *this << std::endl;
            changed = true;
        }
    }

    return SQLExpression::simplify(changed);
}


bool FunctionExpression::isConstant() const {
    for (expression::Expressions::const_iterator j = args_.begin(); j != args_.end(); ++j) {
        if (!(*j)->isConstant()) {
            return false;
        }
    }
    return true;
}

bool FunctionExpression::isAggregate() const {
    for (expression::Expressions::const_iterator j = args_.begin(); j != args_.end(); ++j) {
        if ((*j)->isAggregate()) {
            return true;
        }
    }
    return false;
}

void FunctionExpression::print(std::ostream& s) const {
    s << name_;
    s << '(';
    for (expression::Expressions::const_iterator j = args_.begin(); j != args_.end(); ++j) {
        if (j != args_.begin()) {
            s << ',';
        }
        s << *(*j);
    }
    s << ')';
}

void FunctionExpression::tables(std::set<const SQLTable*>& t) {
    for (expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j) {
        (*j)->tables(t);
    }
}

void FunctionExpression::shiftArgs(int minColumnShift) {
    args_ = args_.reshift_expressions(minColumnShift);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression::function
