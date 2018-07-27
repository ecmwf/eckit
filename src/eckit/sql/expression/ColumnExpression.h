/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Simon Smart
/// ECMWF Dec 03

#ifndef eckit_sql_ColumnExpression_H
#define eckit_sql_ColumnExpression_H

#include "eckit/sql/expression/SQLExpression.h"

namespace eckit {
namespace sql {

class SQLOutput;
class SQLTable;

namespace expression {

//----------------------------------------------------------------------------------------------------------------------

class ColumnExpression : public SQLExpression {
public:
    ColumnExpression(const std::string&, SQLTable*, int begin = -1, int end = -1);
    ColumnExpression(const std::string&, const std::string& tableReference, int begin = -1, int end = -1);
    ColumnExpression(const ColumnExpression&) = default;
	~ColumnExpression(); 

	SQLTable* table() { return table_; }
    double* current() { return value_.first; }
    std::shared_ptr<SQLExpression> clone() const;

	SQLExpression* nominalShift(int n) { nominalShift_ = n; return this; }

protected:
	const type::SQLType*   type_;
    std::pair<double*,bool&>    value_;
	std::string                 columnName_;
	SQLTable*              table_;
	std::string                 tableReference_;
    int                    beginIndex_;
    int                    endIndex_;
    int                    nominalShift_;

// -- Overridden methods
	virtual void print(std::ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);
	virtual double eval(bool& missing) const;
	virtual bool isConstant() const { return false; }
	virtual void output(SQLOutput& s) const;

private:
	ColumnExpression& operator=(const ColumnExpression&);

// -- Overridden methods
	virtual const type::SQLType* type() const { return type_; }
    virtual void expandStars(const std::vector<std::reference_wrapper<SQLTable>>&, Expressions&);
	virtual void tables(std::set<SQLTable*>&);

	friend class SQLSelectFactory;

	//friend std::ostream& operator<<(std::ostream& s,const ColumnExpression& p)
	//	{ p.print(s); return s; }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
