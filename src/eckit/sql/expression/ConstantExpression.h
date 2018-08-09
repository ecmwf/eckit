/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// ECMWF Oct 11

#ifndef eckit_sql_ConstantExpression_H
#define eckit_sql_ConstantExpression_H

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/SQLExpression.h"

namespace eckit {
namespace sql {

class SQLSelect;
class SQLTable;
class SQLOutput;

namespace expression {

//----------------------------------------------------------------------------------------------------------------------

class ConstantExpression : public SQLExpression {
public:
    ConstantExpression(double, bool, const type::SQLType*);
	virtual ~ConstantExpression(); 

	virtual void prepare(SQLSelect&) { NOTIMP; }
	virtual void cleanup(SQLSelect&) { NOTIMP; }

	// -- For WHERE
	virtual double eval(bool& missing) const { missing = missing_; return value_; }

	virtual bool andSplit(expression::Expressions&) { return false; }
	virtual void tables(std::set<SQLTable*>&) {}

	virtual bool isConstant() const { return true; }
	virtual bool isNumber() const { NOTIMP; return false; }

	//virtual SQLExpression* simplify(bool&);
	//virtual void title(const std::string&);
	//virtual std::string title() const;

    virtual const type::SQLType* type() const;
	// ----

    virtual std::shared_ptr<SQLExpression> clone() const { NOTIMP; return 0; }
	
	virtual bool isAggregate() const { return false; }
	// For select expression

	virtual void output(SQLOutput& s) const;
	virtual void partialResult() { NOTIMP; }
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&,expression::Expressions&) { NOTIMP; }

	virtual bool isBitfield() const { return isBitfield_; }
	BitfieldDef bitfieldDef() const { return bitfieldDef_; }
	virtual bool hasMissingValue() const { return hasMissingValue_; }
	double missingValue() const { return missingValue_; }

protected:
	virtual void print(std::ostream&) const { NOTIMP; }; 

	bool isBitfield_;
	BitfieldDef bitfieldDef_;
	bool hasMissingValue_;
	double missingValue_;

private:
	ConstantExpression(const ConstantExpression&);
	ConstantExpression& operator=(const ConstantExpression&);

	double value_;
	bool missing_;
    const type::SQLType& type_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace expression
} // namespace sql
} // namespace eckit


#endif
