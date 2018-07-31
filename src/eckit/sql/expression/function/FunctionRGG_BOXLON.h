/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionRGG_BOXLON.h
// ECMWF July 2010

#ifndef FunctionRGG_BOXLON_H
#define FunctionRGG_BOXLON_H

#include "odb_api/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionRGG_BOXLON : public FunctionExpression {
public:
	FunctionRGG_BOXLON(const std::string&,const expression::Expressions&);
	FunctionRGG_BOXLON(const FunctionRGG_BOXLON&);
	~FunctionRGG_BOXLON(); 

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionRGG_BOXLON& operator=(const FunctionRGG_BOXLON&);

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual double eval(bool& missing) const;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionRGG_BOXLON& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
