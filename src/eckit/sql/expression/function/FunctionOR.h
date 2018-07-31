/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionOR.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionOR_H
#define FunctionOR_H

#include "odb_api/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionOR : public FunctionExpression {
public:
	FunctionOR(const std::string&,const expression::Expressions&);
	FunctionOR(const FunctionOR&);
	~FunctionOR();

// -- Overridden methods
	SQLExpression* clone() const;

	virtual double eval(bool& missing) const;
	virtual const eckit::sql::type::SQLType* type() const;
	virtual SQLExpression* simplify(bool&);

private:
// No copy allowed
	FunctionOR& operator=(const FunctionOR&);

// -- Friends
    //friend std::ostream& operator<<(std::ostream& s,const FunctionOR& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
