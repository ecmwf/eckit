/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLSelect.h"

#include <algorithm>

#include "eckit/config/LibEcKit.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Log.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/ConstantExpression.h"
#include "eckit/sql/expression/OrderByExpressions.h"
#include "eckit/sql/expression/SQLExpressionEvaluated.h"
#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

using namespace expression;

SQLSelect::SQLSelect(const Expressions& columns, const std::vector<std::reference_wrapper<const SQLTable>>& tables,
                     std::shared_ptr<SQLExpression> where, SQLOutput& output,
                     std::vector<std::unique_ptr<SQLOutput>>&& ownedOutputs) :
    select_(columns),
    where_(where),
    simplifiedWhere_(0),
    ownedOutputs_(std::move(ownedOutputs)),
    output_(output),
    aggregatedResultsIterator_(aggregatedResults_.end()),
    count_(0),
    total_(0),
    skips_(0),
    aggregate_(false),
    mixedAggregatedAndScalar_(false),
    doOutputCached_(false) {
    // TODO: Convert tables_, allTables_ to use references rather than pointers.
    for (const SQLTable& t : tables) {
        tables_.push_back(&t);
    }
}

SQLSelect::~SQLSelect() {}


const SQLTable& SQLSelect::findTable(const std::string& name) const {

    std::set<const SQLTable*> names;

    for (const SQLTable* table : tables_) {
        if (table->hasColumn(name)) {
            names.insert(table);
        }
    }

    if (names.size() == 0) {
        throw eckit::UserError("Can't find a table for", name);
    }

    if (names.size() != 1) {
        throw eckit::UserError("Ambiguous column name", name);
    }

    Log::debug<LibEcKit>() << "SQLSelect::findTable: name='" << name << "'" << std::endl;

    return **names.begin();
}


void SQLSelect::ensureFetch(const SQLTable& table, const std::string& columnName) {
    // Add the column to the fetch list associated with the table

    const SQLColumn& column(table.column(columnName));
    std::string fullname = column.fullName();

    allTables_.insert(&table);
    if (tablesToFetch_.find(&table) == tablesToFetch_.end()) {
        tablesToFetch_[&table] = SelectOneTable(&table);
    }

    auto& fetch(tablesToFetch_[&table].fetch_);
    if (std::find_if(fetch.begin(), fetch.end(), [&](const SQLColumn& c) { return &c == &column; }) == fetch.end()) {
        fetch.push_back(column);
        //        tablesToFetch_[&table].fetchSizeDoubles_.push_back(column.dataSizeDoubles());
        // This will create the value if it doesn't exist
        ValueLookup& value(values_[fullname]);
        tablesToFetch_[&table].values_.push_back(&value);
    }
}


SQLSelect::ValueLookup& SQLSelect::column(const std::string& name, const SQLTable* table) {
    // if this ASSERT fails, we are likely missing a call to preprepare() on the expression
    // in SQlSelect::prepareExecute()
    ASSERT(table);
    const SQLColumn& column(table->column(name));

    std::string fullname = column.fullName();
    Log::debug<LibEcKit>() << "Accessing column " << fullname << std::endl;

    auto it = values_.find(fullname);
    ASSERT(it != values_.end());

    return it->second;
}

const type::SQLType* SQLSelect::typeOf(const std::string& name, const SQLTable* table) const {
    if (!table) {
        table = &findTable(name);
    }
    const SQLColumn& column(table->column(name));

    const type::SQLType& type = column.type();
    return type.subType(name);  // This should take care of bitfields
}

static bool compareTables(SelectOneTable* a, SelectOneTable* b) {
    // #if 1
    //	if(&(a->table_->owner()) != &(b->table_->owner()))
    return a->table_->owner().name() < b->table_->owner().name();
    // #else
    //	return a->table_->index() < b->table_->index();
    // #endif
}

inline bool SQLSelect::resultsOut() {
    return output_.output(select_);
}

std::shared_ptr<SQLExpression> SQLSelect::findAliasedExpression(const std::string& alias) {
    for (size_t i(0); i < select_.size(); ++i) {
        if (select_[i]->title() == alias) {
            return select_[i];
        }
    }
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

    if (where_) {
        where_->preprepare(*this);
    }
    output_.preprepare(*this);

    // If no tables have been required, but there are expressions, it implies that we are
    // just using functions that don't depend on the data (e.g. rownumber()). Just ensure
    // that we do something...

    if (!select_.empty() && tablesToFetch_.empty()) {
        for (const SQLTable* table : tables_) {
            tablesToFetch_[table] = SelectOneTable(table);
        }
    }

    // Construct the cursors that will be used for the selection, and pass these in to the
    // constructed cursors/iterators

    for (auto& tablePair : tablesToFetch_) {
        SelectOneTable& tbl(tablePair.second);
        // n.b. tablePair.first is only const to enable other functions to be const. But
        //      it belongs to this structure, and we are a non-const fn, so this is ok.
        SQLTable* sqlTable = const_cast<SQLTable*>(tablePair.first);
        cursors_.emplace_back(tbl.table_->iterator(
            tbl.fetch_, [this, sqlTable](SQLTableIterator& cursor) { refreshCursorMetadata(sqlTable, cursor); }));
        cursors_.back()->rewind();

        refreshCursorMetadata(sqlTable, *cursors_.back());
    }

    // Prepare the columns

    for (Expressions::iterator c = select_.begin(); c != select_.end(); ++c) {

        if ((*c)->isAggregate()) {
            aggregated_.push_back(*c);
            mixedResultColumnIsAggregated_.push_back(true);
        }
        else {
            nonAggregated_.push_back(*c);
            mixedResultColumnIsAggregated_.push_back(false);
        }

        (*c)->prepare(*this);

        Log::debug<LibEcKit>() << "SQLSelect::prepareExecute: '" << *(*c) << "'" << std::endl;
    }

    ASSERT(select_.size() == mixedResultColumnIsAggregated_.size());
    ASSERT(select_.size() == aggregated_.size() + nonAggregated_.size());

    output_.prepare(*this);

    if (aggregated_.size()) {
        aggregate_ = true;
        Log::debug<LibEcKit>() << "SELECT is aggregated" << std::endl;

        if (aggregated_.size() != select_.size()) {
            mixedAggregatedAndScalar_ = true;
            Log::debug<LibEcKit>() << "SELECT has aggregated and non-aggregated results" << std::endl;
        }
    }


    std::shared_ptr<SQLExpression> where(where_);
    if (where) {
        where->prepare(*this);

        bool more = true;
        while (more) {
            more = false;
            std::shared_ptr<SQLExpression> w(where->simplify(more));
            if (w) {
                where = w;
            }
            simplifiedWhere_ = where;
        }

        Log::debug<LibEcKit>() << "Simplified WHERE " << *where << std::endl;
        if (where->isConstant()) {
            bool missing = false;
            if (where->eval(missing)) {
                Log::debug<LibEcKit>() << "WHERE condition always true" << std::endl;
                where = 0;
            }
            else {
                Log::debug<LibEcKit>() << "WHERE condition always false" << std::endl;
                return;
            }
        }
    }

    // Check for links
    for (std::set<const SQLTable*>::iterator j = allTables_.begin(); j != allTables_.end(); ++j) {
        const SQLTable* table1   = *j;
        const std::string& name1 = table1->name();

        for (std::set<const SQLTable*>::iterator k = allTables_.begin(); k != allTables_.end(); ++k) {
            const SQLTable* table2 = *k;
            SelectOneTable& x      = tablesToFetch_[table2];

            if (table1->hasLinkTo(*table2)) {
                const std::string& name2 = table2->name();

                // That can happen for 'aligned' tables
                if (x.column_) {
                    ASSERT(table2 == x.table2_);
                    Log::warning() << "Ignoring link " << name1 << "->" << name2 << ", using " << x.table1_->fullName()
                                   << "->" << x.table2_->fullName() << std::endl;
                    continue;
                }
                Log::debug<LibEcKit>() << "Using link " << table1->fullName() << "->" << table2->fullName() << std::endl;

                //
                std::string o      = name2 + ".offset";
                ValueLookup offset = column(o, table1);

                std::string l      = name2 + ".length";
                ValueLookup length = column(l, table1);

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

    for (TableMap::iterator j = tablesToFetch_.begin(); j != tablesToFetch_.end(); ++j) {
        sortedTables_.push_back(&(*j).second);
    }

    if (where) {
        // Try to analyse where
        expression::Expressions e;
        if (!where->andSplit(e)) {
            e.push_back(where);
        }

        for (size_t i = 0; i < e.size(); ++i) {
            Log::debug<LibEcKit>() << "WHERE AND split " << *(e[i]) << std::endl;

            // Get tables accessed
            std::set<const SQLTable*> t;
            e[i]->tables(t);

            for (std::set<const SQLTable*>::iterator j = t.begin(); j != t.end(); ++j) {
                Log::debug<LibEcKit>() << "  tables -> " << (*j)->fullName() << std::endl;
            }

            if (t.size() == 1) {
                const SQLTable* table = *(t.begin());

                tablesToFetch_[table].check_.push_back(e[i]);
                Log::debug<LibEcKit>() << "WHERE quick check for " << table->fullName() << " " << (*e[i]) << std::endl;
            }
        }
    }

    // Needed, for example, if we do: select count(*) from "file.oda"
    if (sortedTables_.size() == 0) {
        for (std::vector<const SQLTable*>::iterator i = tables_.begin(); i != tables_.end(); ++i) {
            sortedTables_.push_back(new SelectOneTable(*i));  // TODO: release the objects!
        }
    }

    std::sort(sortedTables_.begin(), sortedTables_.end(), compareTables);
    Log::debug<LibEcKit>() << "TABLE order " << std::endl;
    for (SortedTables::iterator k = sortedTables_.begin(); k != sortedTables_.end(); ++k) {
        Log::debug<LibEcKit>() << (*k)->table_->fullName() << " " << (*k)->order_ << std::endl;

        for (size_t i = 0; i < (*k)->check_.size(); i++) {
            Log::debug<LibEcKit>() << "    QUICK CHECK " << *((*k)->check_[i]) << std::endl;
        }

        for (size_t i = 0; i < (*k)->index_.size(); i++) {
            Log::debug<LibEcKit>() << "    INDEX CHECK " << *((*k)->index_[i]) << std::endl;
        }
    }


    // Add the multi-table quick checks
    if (where) {
        expression::Expressions e;
        if (!where->andSplit(e)) {
            e.push_back(where);
        }

        std::set<const SQLTable*> ordered;
        for (SortedTables::iterator k = sortedTables_.begin(); k != sortedTables_.end(); ++k) {
            const SQLTable* table = (*k)->table_;
            ordered.insert(table);

            for (size_t i = 0; i < e.size(); ++i) {
                if (e[i]) {
                    // Get tables accessed
                    std::set<const SQLTable*> t;
                    e[i]->tables(t);

                    if (t.size() != 1) {
                        bool ok = true;
                        for (std::set<const SQLTable*>::iterator j = t.begin(); j != t.end(); ++j) {
                            if (ordered.find(*j) == ordered.end()) {
                                ok = false;
                            }
                        }

                        if (ok) {
                            (*k)->check_.push_back(e[i]);
                            Log::debug<LibEcKit>() << "WHERE multi-table quick check for " << table->fullName() << " " << (*e[i])
                                                   << std::endl;

                            e[i] = 0;
                        }
                    }
                    else {
                        e[i] = 0;
                    }
                }
            }
        }

        // Add what's left to last table
        for (size_t i = 0; i < e.size(); ++i) {
            if (e[i]) {
                sortedTables_.back()->check_.push_back(e[i]);
            }
        }
        where = 0;
    }

    // Debug output

    Log::debug<LibEcKit>() << "SQLSelect:prepareExecute: TABLE order:" << std::endl;
    for (SortedTables::iterator k = sortedTables_.begin(); k != sortedTables_.end(); ++k) {
        Log::debug<LibEcKit>() << "SQLSelect:prepareExecute: TABLE order " << (*k)->table_->fullName() << " "
                               << (*k)->order_ << std::endl;

        for (size_t i = 0; i < (*k)->check_.size(); i++) {
            Log::debug<LibEcKit>() << "    QUICK CHECK " << *((*k)->check_[i]) << std::endl;
        }
    }
}

unsigned long long SQLSelect::execute() {
    prepareExecute();
    process();
    postExecute();
    return count_;
}

void SQLSelect::postExecute() {

    output_.flush();
    output_.cleanup(*this);
    if (simplifiedWhere_) {
        simplifiedWhere_->cleanup(*this);
    }

    for (expression::Expressions::iterator c(select_.begin()); c != select_.end(); ++c) {
        (*c)->cleanup(*this);
    }

    Log::debug<LibEcKit>() << "Matching row(s): " << BigNum(output_.count()) << " out of " << BigNum(total_)
                           << std::endl;
    Log::debug<LibEcKit>() << "Skips: " << BigNum(skips_) << std::endl;
    reset();
}

void SQLSelect::refreshCursorMetadata(SQLTable* table, SQLTableIterator& cursor) {

    auto it = tablesToFetch_.find(table);

    ASSERT(it != tablesToFetch_.end());
    SelectOneTable& tbl(it->second);

    const double* data(cursor.data());
    const std::vector<size_t> offsets(cursor.columnOffsets());
    const std::vector<size_t> doublesSizes(cursor.doublesDataSizes());
    const std::vector<char> hasMissing(cursor.columnsHaveMissing());
    const std::vector<double> missingValues(cursor.missingValues());

    for (size_t i = 0; i < tbl.fetch_.size(); i++) {
        std::string fullname(tbl.fetch_[i].get().fullName());

        // ASSERT is no longer true if using to refresh values
        // ASSERT(values_.find(fullname) == values_.end());

        // HACK ALERT
        // Our output functionality gets the width of strings from the column type.
        // but the width can change on refreshCursorMetadata
        // --> We need to update the column type.
        // This is a function of the table. Aaaarg. Makes this horribly not parallelisable.
        // eckit::sql needs a rewrite. Will enable us to work around this shit.
        table->updateColumnDoublesWidth(fullname, doublesSizes[i]);

        // Aaaargh, we need to know when we are looking at missing values. But these can change
        // down the column. As can 'hasMissing'
        table->updateColumnMissingValues(fullname, hasMissing[i], missingValues[i]);

        // This will create the value if it does not exist.
        std::pair<const double*, bool>& value(values_[fullname]);
        value.first = &data[offsets[i]];
    }

    for (Expressions::iterator c = select_.begin(); c != select_.end(); ++c) {
        (*c)->updateType(*this);
    }

    if (where_) {
        where_->prepare(*this);
    }

    output_.updateTypes(*this);
}

void SQLSelect::reset() {
    aggregate_                = false;
    mixedAggregatedAndScalar_ = false;
    doOutputCached_           = false;

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
    // if (where) Log::info() << "SQLSelect::output: where: " << *where << std::endl;

    bool newRow  = false;
    bool missing = false;
    double value;
    if (!where || (((value = where->eval(missing)) || !value)  // !value for the 'WHERE 0' case, ODB-106
                   && !missing)) {
        if (!aggregate_) {
            newRow = resultsOut();
        }
        else {
            size_t n = select_.size();
            if (!mixedAggregatedAndScalar_) {
                for (size_t i = 0; i < n; i++) {
                    select_[i]->partialResult();
                }
            }
            else {

                // For each set of non-aggregated values, keep track of the aggregated values
                // n.b. newRow=false, as we are accumulating the values

                OrderByExpressions nonAggregatedValues;
                for (size_t i = 0; i < nonAggregated_.size(); ++i) {
                    nonAggregatedValues.emplace_back(std::make_shared<SQLExpressionEvaluated>(*nonAggregated_[i]));
                }

                AggregatedResults::iterator results = aggregatedResults_.find(nonAggregatedValues);
                if (results == aggregatedResults_.end()) {
                    Expressions& aggregated = aggregatedResults_[nonAggregatedValues];
                    for (const auto& expr : aggregated_) {
                        aggregated.emplace_back(expr->clone());
                    }
                }

                Expressions& aggregated = aggregatedResults_[nonAggregatedValues];
                for (size_t i = 0; i < aggregated.size(); ++i) {
                    aggregated[i]->partialResult();
                }
            }
        }
    }
    return newRow;
}


unsigned long long SQLSelect::process() {

    ASSERT(cursors_.size() != 0);
    ASSERT(count_ == 0);

    while (processOneRow()) {
        /* Intentionally blank */;
    }

    return count_;
}


bool SQLSelect::processNextTableRow(size_t tableIndex) {

    ASSERT(cursors_.size() > tableIndex);
    ASSERT(cursors_.size() == sortedTables_.size());

    /// For one table, obtain the next row that also validates, or return false if there is not one.

    SelectOneTable& fetchTable(*sortedTables_[tableIndex]);

    total_++;

    while (cursors_[tableIndex]->next()) {

        // Extract the missing values

        for (size_t i = 0; i < fetchTable.fetch_.size(); i++) {
            fetchTable.values_[i]->second = fetchTable.fetch_[i].get().isMissingValue(fetchTable.values_[i]->first);
        }

        // Test thereturned row against the validation conditions.

        bool ok = true;

        for (auto& check : fetchTable.check_) {
            bool missing = false;
            if (!check->eval(missing) || missing) {
                ok = false;
                break;
            }
        }

        if (ok) {
            return true;
        }

        skips_++;
        total_++;
    }

    // If no row was found, then ensure total_ was not incremented.
    total_--;

    return false;
}


bool SQLSelect::processOneRow() {

    // n.b. it is acceptable for fromTables.size() == 0, if the expressions
    //      are all constant. So we just check the number of used tables.
    ASSERT(cursors_.size() == sortedTables_.size());

    // If we are using and output iterator that caches the output, and are now replaying
    // that data (say using OrderBy), then do that.

    if (doOutputCached_) {
        if (output_.cachedNext()) {
            count_++;
            return true;
        }
        doOutputCached_ = false;
        return false;
    }

    // If this is the first retrieve, we need to initialise all tables

    if (count_ == 0) {
        for (size_t idx = 0; idx < cursors_.size(); idx++) {
            if (!processNextTableRow(idx)) {
                return false;  // If false, there is no data
            }
        }

        if (writeOutput()) {
            count_++;
            return true;
            ;
        }
    }

    // Otherwise, start by incrementing the first table. If that is exhausted, reset that table
    // and increment the second, and continue until we have enumerated all possible combinations
    // of valid data across the tables.

    if (!mixedAggregatedAndScalar_ || aggregatedResultsIterator_ == aggregatedResults_.end()) {

        for (size_t idx = 0; idx < cursors_.size(); idx++) {

            // n.b. keep going until writeOutput() has done something - i.e. a row has been
            // returned. This allows us to have filtering/unique/aggregation in the Output
            while (processNextTableRow(idx)) {
                if (writeOutput()) {
                    count_++;
                    return true;
                }
            }

            // If we have exhausted the available rows, rewind and increment those in the next table

            if (idx != cursors_.size() - 1) {
                cursors_[idx]->rewind();
                ASSERT(processNextTableRow(idx));
            }
        }
    }

    // If we are considering mixed aggregate/non-aggregate results, then we need to output
    // them here
    // We put this here rather than in postExecute such that the Select class in odb can
    // iterate over one entry at a time.

    if (aggregatedResultsIterator_ == aggregatedResults_.end()) {
        aggregatedResultsIterator_ = aggregatedResults_.begin();
    }
    else {
        ++aggregatedResultsIterator_;
    }
    while (aggregatedResultsIterator_ != aggregatedResults_.end()) {
        const OrderByExpressions& nonAggregated = aggregatedResultsIterator_->first;
        Expressions& aggregated                 = aggregatedResultsIterator_->second;
        Expressions results;
        size_t ai = 0;
        size_t ni = 0;
        for (size_t i = 0; i < mixedResultColumnIsAggregated_.size(); i++) {
            if (mixedResultColumnIsAggregated_[i]) {
                results.push_back(aggregated[ai++]);
            }
            else {
                results.push_back(nonAggregated[ni++]);
            }
        }

        if (output_.output(results)) {
            count_++;
            return true;
        }

        ++aggregatedResultsIterator_;
    }

    // If this is an aggregate (not mixed aggregate) case, then we are done the
    // first time we pass through. But ensure that the caller is told that there
    // is at least some data!

    if (aggregate_ && !mixedAggregatedAndScalar_ && count_ == 0) {
        resultsOut();
        count_++;
        return true;
    }

    // If we still get here, we might be using an output that caches all the results
    // (e.g. OrderBy). Give it the chance to do its output

    if (output_.cachedNext()) {
        doOutputCached_ = true;
        count_++;
        return true;
    }

    // Nothing to return.

    return false;
}


void SQLSelect::print(std::ostream& s) const {
    s << "SELECT";
    char sep = ' ';

    for (expression::Expressions::const_iterator c = select_.begin(); c != select_.end(); ++c) {
        s << sep << *(*c);
        sep = ',';
    }

    s << " FROM";
    sep = ' ';
    for (std::vector<const SQLTable*>::const_iterator t = tables_.begin(); t != tables_.end(); ++t) {
        s << sep << (*t)->name();
        sep = ',';
    }

    if (where_.get()) {
        s << " WHERE " << *where_;
    }

    s << " " << output_;
}

expression::Expressions SQLSelect::output() const {
    return select_;
}

std::vector<PathName> SQLSelect::outputFiles() const {
    return outputFiles_;
}
void SQLSelect::outputFiles(const std::vector<PathName>& files) {
    outputFiles_ = files;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql
