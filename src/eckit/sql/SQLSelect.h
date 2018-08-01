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
/// @author Simon SMart
/// ECMWF Dec 03

#ifndef eckit_sql_SQLSelect_H
#define eckit_sql_SQLSelect_H

#include "eckit/filesystem/PathName.h"

#include "eckit/sql/SelectOneTable.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/Environment.h"

namespace eckit {
	class SelectIterator;
    namespace sql {
        class SQLTableIterator;
        namespace expression {
            namespace function {
                class FunctionROWNUMBER;
                class FunctionTHIN;
            }
        }
    }
}


namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------


class SQLSelect : public SQLStatement {
    friend class SelectIterator;

public:

    typedef std::pair<const double*, bool&> ValueLookup;

    SQLSelect(const Expressions& columns,
              std::vector<std::reference_wrapper<SQLTable>>& tables,
              std::shared_ptr<expression::SQLExpression> where,
              SQLOutput& out,
              bool all);
	~SQLSelect(); 

// -- Methods
	void prepareExecute(); //SQLExpression*& where);
    unsigned long long process();
    bool processOneRow();
    void postExecute();

    bool isAggregate() { return aggregate_; }

    ValueLookup column(const std::string& name, SQLTable*);
    const type::SQLType* typeOf(const std::string& name, SQLTable*);
	// FIXME: do we really need all these optional parameters?
    SQLTable& findTable(const std::string& name, std::string *fullName = 0, bool *hasMissingValue=0, double *missingValue=0, bool* isBitfield=0, BitfieldDef* =0) const;
    void ensureFetch(SQLTable& table, const std::string& columnName);

	virtual Expressions output() const; 

    std::vector<eckit::PathName> outputFiles() const;
    void outputFiles(const std::vector<eckit::PathName>& files);
    bool all() const { return all_; }
    std::vector<SQLTable*>& tables() { return tables_; }
    expression::SQLExpression* where() { return where_.get(); }

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

    std::shared_ptr<expression::SQLExpression> where_;
    std::shared_ptr<expression::SQLExpression> simplifiedWhere_;

    // Cursors provide the environment for the iteration over the tables
    std::vector<std::unique_ptr<SQLTableIterator>> cursors_;

    SQLOutput& output_;

    typedef std::map<
        std::vector<std::pair<double,bool>>,
        std::shared_ptr<expression::Expressions>
    > AggregatedResults;

	AggregatedResults aggregatedResults_;

    // n.b. we don't use std::vector<bool> as you cannot take a reference to a single element.

    std::map<std::string, std::pair<const double*, bool>> values_;

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
    std::vector<eckit::PathName> outputFiles_;
    bool all_;

// -- Methods

	void reset();
    bool resultsOut();
    bool writeOutput();
    std::shared_ptr<SQLExpression> findAliasedExpression(const std::string& alias);

    bool processNextTableRow(size_t tableIndex);

    friend class expression::function::FunctionROWNUMBER; // needs access to count_
    friend class expression::function::FunctionTHIN; // needs access to count_

	friend std::ostream& operator<<(std::ostream& s,const SQLSelect& p)
		{ p.print(s); return s; }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace eckit

#endif
