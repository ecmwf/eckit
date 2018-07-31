/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionRGG_BOXLAT.h
/// ECMWF July 2010

#ifndef FunctionRGG_BOXLAT_H
#define FunctionRGG_BOXLAT_H

#include "odb_api/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionRGG_BOXLAT : public FunctionExpression {
public:
	FunctionRGG_BOXLAT(const std::string&, const expression::Expressions&);
	FunctionRGG_BOXLAT(const FunctionRGG_BOXLAT&);
	~FunctionRGG_BOXLAT(); 

	SQLExpression* clone() const;
private:
// No copy allowed
	FunctionRGG_BOXLAT& operator=(const FunctionRGG_BOXLAT&);

// -- Overridden methods
	virtual double eval(bool& missing) const;
	virtual const eckit::sql::type::SQLType* type() const;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionRGG_BOXLAT& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit 

#endif
