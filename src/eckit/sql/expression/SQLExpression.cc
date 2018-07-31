/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/SQLExpression.h"
#include "eckit/sql/expression/NumberExpression.h"
//#include "eckit/sql/type/SQLType.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/expression/SQLExpressions.h"

using namespace eckit;

namespace eckit {
namespace sql {
namespace expression {

double const MISSING_VALUE_REAL = -2147483647.0;
long const MISSING_VALUE_INT =  2147483647;

SQLExpression::SQLExpression()
: isBitfield_(false), hasMissingValue_(false), missingValue_(MISSING_VALUE_REAL)
{}

SQLExpression::~SQLExpression() {}

bool SQLExpression::isVector() const { return false; }

Expressions& SQLExpression::vector()
{
	NOTIMP;
	return *((Expressions*) 0);
}

Dictionary& SQLExpression::dictionary()
{
	NOTIMP;
	return *((Dictionary*) 0);
}

std::shared_ptr<SQLExpression> SQLExpression::number(double value) { return std::make_shared<NumberExpression>(value); }

std::shared_ptr<SQLExpression> SQLExpression::simplify(bool& changed)
{
	if(isConstant() && !isNumber())
	{
		changed = true;
		bool missing = false;
		Log::info() << "SIMPLIFY " << *this << " to " << eval(missing) << std::endl;
        return std::make_shared<NumberExpression>(eval(missing));
	}
	return 0;
}

void SQLExpression::output(SQLOutput& s) const 
{ 
	bool missing = false;
	double d = eval(missing); 
	s.outputReal(d, missing);
}

void SQLExpression::title(const std::string& t)
{
	title_ = t;
}

std::string SQLExpression::title() const
{
	if(title_.size())
		return title_;

    std::ostringstream s;
    s << *this;

    return s.str();
}


//const type::SQLType* SQLExpression::type() const { const type::SQLType& x = type::SQLType::lookup("real"); return &x; }

void SQLExpression::expandStars(const std::vector<std::reference_wrapper<SQLTable>>&, expression::Expressions& e)
{
    e.push_back(shared_from_this());
}

} // namespace expression
} // namespace sql
} // namespace eckit

