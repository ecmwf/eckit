/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionEQ_BOXLON.h
// ECMWF July 2010

#ifndef FunctionEQ_BOXLON_H
#define FunctionEQ_BOXLON_H

#include "odb_api/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionEQ_BOXLON : public FunctionExpression {
public:
	FunctionEQ_BOXLON(const std::string&,const expression::Expressions&);
	FunctionEQ_BOXLON(const FunctionEQ_BOXLON&);
	~FunctionEQ_BOXLON(); 

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionEQ_BOXLON& operator=(const FunctionEQ_BOXLON&);

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual double eval(bool& missing) const;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionEQ_BOXLON& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
