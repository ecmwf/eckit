/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionEQ_BOXLAT.h
/// ECMWF July 2010

#ifndef FunctionEQ_BOXLAT_H
#define FunctionEQ_BOXLAT_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionEQ_BOXLAT : public FunctionExpression {
public:
	FunctionEQ_BOXLAT(const std::string&, const expression::Expressions&);
	FunctionEQ_BOXLAT(const FunctionEQ_BOXLAT&);
	~FunctionEQ_BOXLAT();

	std::shared_ptr<SQLExpression> clone() const;

    static int arity() { return 3; }

private:
// No copy allowed
	FunctionEQ_BOXLAT& operator=(const FunctionEQ_BOXLAT&);

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual double eval(bool& missing) const;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionEQ_BOXLAT& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit 

#endif
