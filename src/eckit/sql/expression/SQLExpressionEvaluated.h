/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLExpressionEvaluated.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef SQLExpressionEvaluated_H
#define SQLExpressionEvaluated_H

#include "eckit/sql/SQLExpression.h"

namespace eckit {
namespace sql {
namespace expression {

class SQLExpressionEvaluated : public SQLExpression {
public:
	SQLExpressionEvaluated(SQLExpression&);
	~SQLExpressionEvaluated(); 

	// Overriden

    virtual void prepare(SQLSelect&);
    virtual void cleanup(SQLSelect&);
    virtual double eval(bool& missing) const ;
    virtual bool isConstant() const ;
    virtual bool isNumber() const ;
    virtual SQLExpression* simplify(bool&) ;
    virtual SQLExpression* clone() const;
    virtual bool isAggregate() const ;

    virtual const odb::sql::type::SQLType* type() const ;

    virtual void output(SQLOutput& o) const ;

protected:
	virtual void print(std::ostream&) const;

private:
	SQLExpressionEvaluated(const SQLExpressionEvaluated&);
	SQLExpressionEvaluated& operator=(const SQLExpressionEvaluated&);

	friend std::ostream& operator<<(std::ostream& s, const SQLExpressionEvaluated& p) { p.print(s); return s; }

	const odb::sql::type::SQLType* type_;
	bool missing_;
	double value_;
	double missingValue_;
};

} // namespace expression
} // namespace sql
} // namespace eckit


#endif
