/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionDOTP.h
// ECMWF July 2010

#ifndef FunctionDOTP_H
#define FunctionDOTP_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionDOTP : public FunctionExpression {
public:
	FunctionDOTP(const std::string&,const expression::Expressions&);
	FunctionDOTP(const FunctionDOTP&);
	~FunctionDOTP(); 

	std::shared_ptr<SQLExpression> clone() const;

private:
// No copy allowed
	FunctionDOTP& operator=(const FunctionDOTP&);

	double value_;

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;

	bool isAggregate() const { return true; }

	bool resultNULL_;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionDOTP& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
