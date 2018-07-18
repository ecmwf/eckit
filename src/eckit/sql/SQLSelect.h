/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLSelect.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLSelect_H
#define SQLSelect_H

#include "eckit/filesystem/PathName.h"

#include "odb_api/SelectOneTable.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "eckit/sql/SQLStatement.h"
#include "odb_api/Stack.h"

namespace eckit {

	class SelectIterator;

namespace sql {

namespace expression {
    namespace function {
        class FunctionROWNUMBER;
        class FunctionTHIN;
    }
}

class SQLTableIterator;


class SQLSelect : public SQLStatement {
	friend class odb::SelectIterator;

public:
	SQLSelect(const Expressions&, const std::vector<SQLTable*>&, odb::sql::expression::SQLExpression*, SQLOutput*, SQLOutputConfig, bool all);
	~SQLSelect(); 

// -- Methods
	void prepareExecute(); //SQLExpression*& where);
    unsigned long long process(odb::sql::expression::SQLExpression*,SortedTables::iterator);
    bool processOneRow();
    void postExecute();

	void pushFirstFrame();

	bool isAggregate() { return aggregate_; }

    std::pair<double*,bool&> column(const std::string& name, SQLTable*);
	const type::SQLType* typeOf(const std::string& name, SQLTable*) const;
	// FIXME: do we really need all these optional parameters?
	SQLTable* findTable(const std::string& name, std::string *fullName = 0, bool *hasMissingValue=0, double *missingValue=0, bool* isBitfield=0, BitfieldDef* =0) const;

	virtual Expressions output() const; 

	Expressions& results() { return results_; }

    std::vector<eckit::PathName> outputFiles() const;
    void outputFiles(const std::vector<eckit::PathName>& files);
    bool all() const { return all_; }
    std::vector<SQLTable*>& tables() { return tables_; }
    odb::sql::expression::SQLExpression* where() { return where_.get(); }

// -- Overridden methods
    unsigned long long execute();

protected:
	virtual void print(std::ostream&) const; 	

private:
// No copy allowed
	SQLSelect(const SQLSelect&);
	SQLSelect& operator=(const SQLSelect&);

// -- Members
	Expressions select_;
	std::vector<SQLTable*> tables_;
	SortedTables sortedTables_;

    std::auto_ptr<odb::sql::expression::SQLExpression> where_;
	odb::sql::expression::SQLExpression* simplifiedWhere_;

	Stack env;

    std::auto_ptr<SQLOutput> output_;
	Expressions  results_;

    typedef std::map<std::vector<std::pair<double,bool> >, expression::Expressions*> AggregatedResults;
	AggregatedResults aggregatedResults_;

    // n.b. we don't use std::vector<bool> as you cannot take a reference to a single element.

    std::map<std::string, std::pair<std::vector<double>, bool>> values_;

    std::set<SQLTable*> allTables_;

	typedef std::map<SQLTable*,SelectOneTable> TableMap;
	TableMap tablesToFetch_;

	unsigned long long count_;
	unsigned long long total_;
	unsigned long long skips_;

	bool aggregate_;
	bool mixedAggregatedAndScalar_;
	Expressions aggregated_;
	Expressions nonAggregated_;
	std::vector<bool> mixedResultColumnIsAggregated_;
	SQLOutputConfig outputConfig_;
    std::vector<eckit::PathName> outputFiles_;
    bool all_;

// -- Methods

	void reset();
    bool resultsOut();
    bool output(odb::sql::expression::SQLExpression*);
    SQLExpression* findAliasedExpression(const std::string& alias);

	friend class odb::sql::expression::function::FunctionROWNUMBER; // needs access to count_
	friend class odb::sql::expression::function::FunctionTHIN; // needs access to count_

	friend std::ostream& operator<<(std::ostream& s,const SQLSelect& p)
		{ p.print(s); return s; }
};

} // namespace sql
} // namespace eckit

#endif
