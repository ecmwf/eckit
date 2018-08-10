/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Nov 11

#ifndef eckit_api_Expressions_H
#define eckit_api_Expressions_H

#include <memory>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/SQLExpression.h"

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

// n.b. shared pointer not unique_ptr.
//      needs to be copyable to be used by YACC
typedef std::vector<std::shared_ptr<SQLExpression>> ExpressionsVector;

class Expressions : public SQLExpression, public ExpressionsVector
{
public:
	Expressions() : ExpressionsVector() {}
    Expressions(size_t i) : ExpressionsVector(i, 0) {}

    Expressions(const Expressions&) = default;
    Expressions& operator=(const Expressions&) = default;

    Expressions(Expressions&&) = default;
    Expressions& operator=(Expressions&&) = default;

    virtual void print(std::ostream& s) const;

	friend std::ostream& operator<<(std::ostream& o, const Expressions& e)
		{ e.print(o); return o; }

//////////////////////////////////////////////////////////////////////////////////////

	virtual void prepare(SQLSelect&)  {}
	virtual void cleanup(SQLSelect&)  {}

	const type::SQLType* type() const;

	// -- For WHERE
	virtual double eval(bool& missing) const  { NOTIMP; }

	virtual bool isConstant() const  { NOTIMP; }
	virtual bool isNumber() const { return false; }
	virtual bool isVector() const { return true; }
	virtual Expressions& vector() { return *this; }

    virtual std::shared_ptr<SQLExpression> simplify(bool&) { return shared_from_this(); }

    virtual std::shared_ptr<SQLExpression> clone() const;
	
	virtual bool isAggregate() const { return false; }
	// For select expression

	virtual void output(SQLOutput&) const { return NOTIMP; }
	virtual void partialResult() {}
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&,expression::Expressions&) { NOTIMP; }
//////////////////////////////////////////////////////////////////////////////////////
};

typedef std::vector<Expressions> VectorOfExpressions;
//class VectorOfExpressions : public vector<Expressions> { public: ~VectorOfExpressions() { for (size_t i=0; i < size(); ++i) ; } };

//----------------------------------------------------------------------------------------------------------------------

} // namespace expression
} // namespace sql
} // namespace eckit

#endif
