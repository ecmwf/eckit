/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionCOUNT.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionCOUNT_H
#define FunctionCOUNT_H

#include "odb_api/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionCOUNT : public FunctionExpression {
public:
	FunctionCOUNT(const std::string&,const expression::Expressions&);
	FunctionCOUNT(const FunctionCOUNT&);
	~FunctionCOUNT();

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionCOUNT& operator=(const FunctionCOUNT&);

	unsigned long long count_;

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;

	bool isAggregate() const { return true; }

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionCOUNT& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
