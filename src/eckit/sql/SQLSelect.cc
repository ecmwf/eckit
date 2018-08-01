/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/BigNum.h"
#include "eckit/log/Log.h"
#include "eckit/sql/expression/ConstantExpression.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"


namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

using namespace expression;

SQLSelect::SQLSelect(const Expressions& columns, 
    std::vector<std::reference_wrapper<SQLTable>>& tables,
    std::shared_ptr<SQLExpression> where,
    SQLOutput& output,
    bool all)
: select_(columns),
  where_(where),
  simplifiedWhere_(0),
  output_(output),
  count_(0),
  total_(0),
  skips_(0),
  aggregate_(false),
  mixedAggregatedAndScalar_(false),
  all_(all)
{
    // TODO: Convert tables_, allTables_ to use references rather than pointers.
    for (SQLTable& t : tables) tables_.push_back(&t);
}

SQLSelect::~SQLSelect() {}


SQLTable& SQLSelect::findTable(const std::string& name,
                               std::string *fullName,
                               bool *hasMissingValue,
                               double *missingValue,
                               bool* isBitfield,
                               BitfieldDef* bitfieldDef) const {

	std::set<SQLTable*> names;

    for(std::vector<SQLTable*>::const_iterator t = tables_.begin();
		t != tables_.end() ; ++t)
	{
		SQLTable* table = const_cast<SQLTable*>(*t);
		if(table->hasColumn(name, fullName))
		{
            //if(table->hasColumn(name, fullName, hasMissingValue, missingValue, bitfieldDef))
			names.insert(table);
            SQLColumn& column(table->column(name));
			if (hasMissingValue) *hasMissingValue = column.hasMissingValue();
			if (missingValue) *missingValue = column.missingValue();
			if (isBitfield) *isBitfield = column.isBitfield();
			if (bitfieldDef) *bitfieldDef = column.bitfieldDef();

        }
	}

	if(names.size() == 0)
		throw eckit::UserError("Can't find a table for", name);

	if(names.size() != 1)
		throw eckit::UserError("Ambiguous column name", name);

	Log::debug() << "SQLSelect::findTable: name='" << name << "', fullName=" << (fullName ? (std::string("'")+ *fullName+"'") : "") << std::endl;

    return **names.begin();
}


void SQLSelect::ensureFetch(SQLTable& table, const std::string& columnName) {
    // Add the column to the fetch list associated with the table

    SQLColumn& column(table.column(columnName));
    std::string fullname = column.fullName();

    allTables_.insert(&table);
    if (tablesToFetch_.find(&table) == tablesToFetch_.end())
        tablesToFetch_[&table] = SelectOneTable(&table);

    auto& fetch(tablesToFetch_[&table].fetch_);
    if (std::find_if(fetch.begin(), fetch.end(), [&](SQLColumn& c){ return &c == &column; }) == fetch.end()) {
        fetch.push_back(column);
        tablesToFetch_[&table].fetchSizeDoubles_.push_back(column.dataSizeDoubles());
    }
}


SQLSelect::ValueLookup SQLSelect::column(const std::string& name, SQLTable* table)
{
    ASSERT(table);
    SQLColumn& column(table->column(name));

    std::string fullname = column.fullName();
    Log::debug() << "Accessing column " << fullname << std::endl;

    auto it = values_.find(fullname);
    ASSERT(it != values_.end());

    return ValueLookup(it->second.first, it->second.second);
}

const type::SQLType* SQLSelect::typeOf(const std::string& name, SQLTable* table)
{
    if(!table) table = &findTable(name);
    SQLColumn& column(table->column(name));

    const type::SQLType& type = column.type();
	return type.subType(name); // This should take care of bitfields

}

static bool compareTables(SelectOneTable* a,SelectOneTable *b)
{
//#if 1
//	if(&(a->table_->owner()) != &(b->table_->owner()))
		return a->table_->owner().name() < b->table_->owner().name();
//#else
//	return a->table_->index() < b->table_->index();
//#endif
}

inline bool SQLSelect::resultsOut()
{
    return output_.output(select_);
}

std::shared_ptr<SQLExpression> SQLSelect::findAliasedExpression(const std::string& alias)
{
    for (size_t i(0); i < select_.size(); ++i)
        if (select_[i]->title() == alias)
            return select_[i];
    return 0;
}

void SQLSelect::prepareExecute() {
	reset();

    // Associate ColumnExpressions, SQLTable and SQLColumns.
    // n.b. it is a bit yucky to do the prepare() in two steps, but this allows us to
    //      allocate buffers and associate them with iterators appropriately.

    for (Expressions::iterator c = select_.begin(); c != select_.end(); ++c) {
        (*c)->preprepare(*this);
    }

    if(where_) where_->preprepare(*this);

    // Construct the cursors that will be used for the selection, and pass these in to the
    // constructed cursors/iterators

    for (auto& tablePair : tablesToFetch_) {
        SelectOneTable& tbl(tablePair.second);
        cursors_.emplace_back(tbl.table_->iterator(tbl.fetch_));
        cursors_.back()->rewind();

        const double* data(cursors_.back()->data());
        const std::vector<size_t> offsets(cursors_.back()->columnOffsets());

        for (size_t i = 0; i < tbl.fetch_.size(); i++) {
            std::string fullname(tbl.fetch_[i].get().fullName());
            ASSERT(values_.find(fullname) == values_.end());

            std::pair<const double*, bool>& newValue(values_[fullname]);
            newValue.first = &data[offsets[i]];
        }
    }

    // Prepare the columns

    for(Expressions::iterator c = select_.begin(); c != select_.end(); ++c) {

        if((*c)->isAggregate()) {
			aggregated_.push_back(*c);
			mixedResultColumnIsAggregated_.push_back(true);
        } else {
			nonAggregated_.push_back(*c);
			mixedResultColumnIsAggregated_.push_back(false);
		}

		(*c)->prepare(*this);

        Log::debug() << "SQLSelect::prepareExecute: '" << *(*c) << "'" << std::endl;
	}

	ASSERT(select_.size() == mixedResultColumnIsAggregated_.size());
	ASSERT(select_.size() == aggregated_.size() + nonAggregated_.size());

    output_.prepare(*this);
    output_.size(select_.size());

	if(aggregated_.size()) {
		aggregate_ = true;
		Log::debug() << "SELECT is aggregated" << std::endl;

        if(aggregated_.size() != select_.size())
		{
			mixedAggregatedAndScalar_ = true;
			Log::info() << "SELECT has aggregated and non-aggregated results" << std::endl;
		}
	}


    std::shared_ptr<SQLExpression> where(where_);
	if(where) {
		where->prepare(*this);
		
		bool more = true;
		while(more) {
			more = false;
            std::shared_ptr<SQLExpression> w(where->simplify(more));
            if (w) where = w;
			simplifiedWhere_ = where;
		}
		
		Log::debug() << "Simplified WHERE " << *where << std::endl;
		if(where->isConstant()) {
			bool missing = false;
			if(where->eval(missing)) {
				Log::info() << "WHERE condition always true, ignoring" << std::endl;
				where = 0;
			} else {
				Log::info() << "WHERE condition always false" << std::endl;
				return;
			}
		}
	}

	// Check for links
	for(std::set<SQLTable*>::iterator j = allTables_.begin(); j != allTables_.end() ; ++j) {
		SQLTable* table1 = *j;
		const std::string& name1    = table1->name();

		for(std::set<SQLTable*>::iterator k = allTables_.begin(); k != allTables_.end() ; ++k) {
			SQLTable* table2 = *k;
            SelectOneTable& x      = tablesToFetch_[table2];

			if(table1->hasLinkTo(*table2)) {
				const std::string& name2    = table2->name();

				// That can happen for 'aligned' tables
				if(x.column_) {
                    ASSERT(table2 == x.table2_);
					Log::warning() << "Ignoring link " << name1             << "->" << name2             << 
					", using "         << x.table1_->fullName() 
						<< "->" << x.table2_->fullName() << std::endl;
					continue;
				}
				Log::info() << "Using link " << table1->fullName() 
					<< "->" << table2->fullName() << std::endl;

					//
				std::string o                  = name2 + ".offset";
                ValueLookup offset = column(o,table1);

				std::string l                  = name2 + ".length";
                ValueLookup length = column(l,table1);

				// There should not be 2 tables with a link on the same table
				
                // TODO: Test and fixme
//				ASSERT(x.offset_ == 0);
//				ASSERT(x.length_ == 0);
                ASSERT(x.offset_.first == 0);
                ASSERT(x.length_.second == 0);
                ASSERT(x.column_ == 0);

				x.offset_ = offset;
				x.length_ = length;
                x.column_ = &table1->column(name2 + ".offset");
				x.table1_ = table1;
				x.table2_ = table2;
			}
		}
	}

	// ------------------------------------

	for(TableMap::iterator j = tablesToFetch_.begin() ; j != tablesToFetch_.end(); ++j)
		sortedTables_.push_back(& (*j).second);

	if(where) {
		// Try to analyse where
		expression::Expressions e;
		if(!where->andSplit(e))
			e.push_back(where);

		for(size_t i = 0 ; i < e.size() ; ++i) {
			Log::debug() << "WHERE AND split " << *(e[i]) << std::endl;

			// Get tables accessed
			std::set<SQLTable*> t;
			e[i]->tables(t);

			for(std::set<SQLTable*>::iterator j = t.begin(); j != t.end(); ++j)
				Log::debug() << "  tables -> " << (*j)->fullName() << std::endl;

				
			if(t.size() == 1) {
				SQLTable* table = *(t.begin());

                tablesToFetch_[table].check_.push_back(e[i]);
                Log::debug() << "WHERE quick check for " << table->fullName() << " " << (*e[i]) << std::endl;
			}
		}
	}

	// Needed, for example, if we do: select count(*) from "file.oda"
	if (sortedTables_.size() == 0)
		for (std::vector<SQLTable*>::iterator i = tables_.begin(); i != tables_.end(); ++i)
			sortedTables_.push_back(new SelectOneTable(*i)); // TODO: release the objects!

	std::sort(sortedTables_.begin(), sortedTables_.end(), compareTables);
	Log::debug() << "TABLE order " << std::endl;
	for(SortedTables::iterator k = sortedTables_.begin(); k != sortedTables_.end(); ++k) {
		Log::debug() << (*k)->table_->fullName() << " " << (*k)->order_ << std::endl;

		for(size_t i = 0; i < (*k)->check_.size(); i++)
			Log::debug() << "    QUICK CHECK " << *((*k)->check_[i]) << std::endl;

		for(size_t i = 0; i < (*k)->index_.size(); i++)
			Log::debug() << "    INDEX CHECK " << *((*k)->index_[i]) << std::endl;
	}


	// Add the multi-table quick checks
	if(where) {
		expression::Expressions e;
		if(!where->andSplit(e))
			e.push_back(where);

		std::set<const SQLTable*> ordered;
		for(SortedTables::iterator k = sortedTables_.begin(); k != sortedTables_.end(); ++k) {
			const SQLTable* table = (*k)->table_;
			ordered.insert(table);

			for(size_t i = 0; i < e.size(); ++i)
				if(e[i]) {
					// Get tables accessed
					std::set<SQLTable*> t;
					e[i]->tables(t);

					if(t.size() != 1) {
						bool ok = true;
						for(std::set<SQLTable*>::iterator j = t.begin() ; j != t.end(); ++j)
							if(ordered.find(*j) == ordered.end())
								ok = false;

						if(ok) {
							(*k)->check_.push_back(e[i]);
							Log::info() << "WHERE multi-table quick check for " << table->fullName() << " " 
								<< (*e[i]) << std::endl;

							e[i] = 0;
						}
					}
					else e[i] = 0;
				}
		}

        // Add what's left to last table
        for(size_t i = 0 ; i < e.size() ; ++i) {
            if(e[i]) {
                sortedTables_.back()->check_.push_back(e[i]);
            }
        }
		where = 0;
	}

    // Debug output

	Log::debug() << "SQLSelect:prepareExecute: TABLE order:" << std::endl;
	for(SortedTables::iterator k = sortedTables_.begin(); k != sortedTables_.end(); ++k)
	{
		Log::debug() << "SQLSelect:prepareExecute: TABLE order " <<  (*k)->table_->fullName() << " " <<
			(*k)->order_ << std::endl;

		for(size_t i = 0; i < (*k)->check_.size(); i++)
			Log::debug() << "    QUICK CHECK " << *((*k)->check_[i]) << std::endl;
	}
}

unsigned long long SQLSelect::execute()
{
	prepareExecute();
    process();
    postExecute();
    return count_;
}

void SQLSelect::postExecute()
{
	if (mixedAggregatedAndScalar_)
	{
		for (AggregatedResults::iterator it (aggregatedResults_.begin()); it != aggregatedResults_.end(); ++it)
		{
            const std::vector<std::pair<double,bool> >& nonAggregatedValues (it->first);
            const Expressions& aggregated (*(it->second));
			Expressions results;
			size_t ai(0), ni(0);
			for (size_t i (0); i < mixedResultColumnIsAggregated_.size(); ++i)
			{
				if (mixedResultColumnIsAggregated_[i])
					results.push_back(aggregated[ai++]->clone());
				else
				{
                    results.push_back(
                            std::make_shared<ConstantExpression>(
                                    nonAggregatedValues[ni].first,
                                    nonAggregatedValues[ni].second,
                                    nonAggregated_[ni]->type()));
					++ni;
				}
			}

            output_.output(results);
            results.clear();
		}
	}
	else if (aggregate_)
	{
        resultsOut();
	}

    output_.flush();
    output_.cleanup(*this);
	if(simplifiedWhere_) simplifiedWhere_->cleanup(*this);
	
    for(expression::Expressions::iterator c (select_.begin()); c != select_.end() ; ++c)
		(*c)->cleanup(*this);

    Log::info() << "Matching row(s): " << BigNum(output_.count()) << " out of " << BigNum(total_) << std::endl;
    Log::info() << "Skips: " << BigNum(skips_) << std::endl;
	reset();
}

void SQLSelect::reset()
{
	aggregate_ = false;
	mixedAggregatedAndScalar_ = false;

	aggregated_.clear();
	nonAggregated_.clear();
	aggregatedResults_.clear();

	mixedResultColumnIsAggregated_.clear();

	values_.clear();

	tablesToFetch_.clear();
	allTables_.clear();

	sortedTables_.clear();

	skips_ = total_ = 0;

    output_.reset();
    cursors_.clear();
	count_ = 0;
}



bool SQLSelect::writeOutput() {

    std::shared_ptr<SQLExpression>& where(simplifiedWhere_);
    //if (where) Log::info() << "SQLSelect::output: where: " << *where << std::endl;

	bool newRow = false;
	bool missing = false;
    double value;
    if ( !where
        || (((value = where->eval(missing)) || !value) // !value for the 'WHERE 0' case, ODB-106 
            && !missing))
	{
		if (! aggregate_)
            newRow = resultsOut();
		else
		{
            size_t n = select_.size();
			if (! mixedAggregatedAndScalar_)
			{
				for(size_t i = 0; i < n; i++)
                    select_[i]->partialResult();
			} else {
                std::vector<std::pair<double,bool> > nonAggregatedValues;
				for (size_t i = 0; i < nonAggregated_.size(); ++i)
				{
					bool missing = false;
					double v = nonAggregated_[i]->eval(missing);
                    nonAggregatedValues.push_back(std::make_pair(v, missing));
				}
	
				AggregatedResults::iterator results = aggregatedResults_.find(nonAggregatedValues);
				if (results == aggregatedResults_.end())
                    aggregatedResults_[nonAggregatedValues] = std::dynamic_pointer_cast<Expressions>(aggregated_.clone());

                Expressions& aggregated = *aggregatedResults_[nonAggregatedValues];
				for (size_t i = 0; i < aggregated.size(); ++i)
					aggregated[i]->partialResult();
			}
		}
	}
	return newRow;
}


unsigned long long SQLSelect::process() {

    ASSERT(cursors_.size() != 0);
    ASSERT(count_ == 0);

    while (processOneRow())
        /* Intentionally blank */ ;

    return count_;
}



bool SQLSelect::processNextTableRow(size_t tableIndex) {

    ASSERT(cursors_.size() > tableIndex);
    ASSERT(cursors_.size() == sortedTables_.size());

    /// For one table, obtain the next row that also validates, or return false if there is not one.

    while (cursors_[tableIndex]->next()) {

        // Test the returned row against the validation conditions.

        bool ok = true;

        for (auto& check : sortedTables_[tableIndex]->check_) {
            bool missing;
            if (!check->eval(missing) || missing) {
                ok = false;
                break;
            }
        }

        if (ok) return true;
        skips_++;
    }

    return false;
}


bool SQLSelect::processOneRow() {

    ASSERT(cursors_.size() > 0);
    ASSERT(cursors_.size() == sortedTables_.size());

    // If this is the first retrieve, we need to initialise all tables

    if (count_ == 0) {
        for (size_t idx = 0; idx < cursors_.size(); idx++) {
            if (!processNextTableRow(idx)) return false; // If false, there is no data
        }

        total_ = 1;

        if (writeOutput()) {
            count_++;
            return true;;
        }
    }

    // Otherwise, start by incrementing the first table. If that is exhausted, reset that table
    // and increment the second, and continue until we have enumerated all possible combinations
    // of valid data across the tables.

    for (size_t idx = 0; idx < cursors_.size(); idx++) {

        if (processNextTableRow(idx)) {
            total_++;
            if (writeOutput()) {
                count_++;
                return true;
            }
        }

        // If we have exhausted the available rows, rewind and increment those in the next table

        if (idx != cursors_.size()-1) {
            cursors_[idx]->rewind();
            ASSERT(processNextTableRow(idx));
        }
    }

    return false;
}


void SQLSelect::print(std::ostream& s) const
{
	s << "SELECT"; char sep = ' ';

	//if(distinct_)
	//	s << " DISTINCT";

	for(expression::Expressions::const_iterator c = select_.begin(); c != select_.end() ; ++c)
	{
		s << sep << *(*c);
		sep = ',';
	}

	s << " FROM";
	sep = ' ';
	for(std::vector<SQLTable*>::const_iterator t = tables_.begin(); t != tables_.end() ; ++t)
	{
		s << sep << (*t)->name();
		sep = ',';
	}

	if(where_.get())
		s << " WHERE " << *where_;

    s << " " << output_;
}

expression::Expressions SQLSelect::output() const { return select_; }

std::vector<PathName> SQLSelect::outputFiles() const { return outputFiles_; }
void SQLSelect::outputFiles(const std::vector<PathName>& files) { outputFiles_ = files; }

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql 
} // namespace eckit 
