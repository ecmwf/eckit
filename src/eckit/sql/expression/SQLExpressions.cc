/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit {
namespace sql {
namespace expression {

const type::SQLType* Expressions::type() const {
    return &type::SQLType::lookup("real");
}


std::shared_ptr<SQLExpression> Expressions::clone() const {
    std::shared_ptr<Expressions> r = std::make_shared<Expressions>(this->size());
    for (size_t i = 0; i < this->size(); ++i)
        (*r)[i] = (*this)[i]->clone();

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

}  // namespace expression
}  // namespace sql
}  // namespace eckit
