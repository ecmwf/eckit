/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionTIMESTAMP.h
// ECMWF July 2010

#ifndef FunctionTIMESTAMP_H
#define FunctionTIMESTAMP_H

#include "odb_api/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionTIMESTAMP : public FunctionExpression {
public:
	FunctionTIMESTAMP(const std::string&,const expression::Expressions&);
	FunctionTIMESTAMP(const FunctionTIMESTAMP&);
	~FunctionTIMESTAMP(); 

	SQLExpression* clone() const;

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual void output(std::ostream& s) const;

private:
	FunctionTIMESTAMP& operator=(const FunctionTIMESTAMP&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionTIMESTAMP& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
