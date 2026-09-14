// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit::sql::expression {

const type::SQLType* Expressions::type() const {
    return &type::SQLType::lookup("real");
}

std::shared_ptr<SQLExpression> Expressions::clone() const {
    std::shared_ptr<Expressions> r = std::make_shared<Expressions>(this->size());
    for (size_t i = 0; i < this->size(); ++i) {
        (*r)[i] = (*this)[i]->clone();
    }

    return r;
}

std::shared_ptr<SQLExpression> Expressions::reshift(int minColumnShift_) const {
    // This is almost certainly not what you are trying to do here.
    // See reshift_expressions
    NOTIMP;
}

Expressions Expressions::reshift_expressions(int minColumnShift) const {
    Expressions shifted;
    for (auto& e : *this) {
        shifted.emplace_back(e->reshift(minColumnShift));
    }
    return shifted;
}

void Expressions::print(std::ostream& o) const {
    o << "[";
    for (size_t i = 0; i < size(); ++i) {
        at(i)->print(o);
        o << ",";
    }
    o << "]";
}

}  // namespace eckit::sql::expression
