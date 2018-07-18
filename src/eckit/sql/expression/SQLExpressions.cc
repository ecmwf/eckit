/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/Expressions.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit {
namespace sql {
namespace expression {

const type::SQLType* Expressions::type() const { return &type::SQLType::lookup("real"); }

Expressions& Expressions::operator=(const Expressions& e)
{
	ExpressionsVector::operator=(e);
	return *this;
}

//Expressions * Expressions::clone() const
SQLExpression * Expressions::clone() const
{
	Expressions *r = new Expressions(this->size());
	for (size_t i = 0; i < this->size(); ++i)
		(*r)[i] = (*this)[i]->clone();

	return r;
}

void Expressions::release()
{
	for (size_t i = 0; i < this->size(); ++i)
		delete at(i);
}

void Expressions::print(std::ostream& o) const
{
	o << "[";
	for (size_t i = 0; i < size(); ++i)
	{
		at(i)->print(o);
		o << ",";
	}
	o << "]";
}

} // namespace expression
} // namespace sql
} // namespace eckit

