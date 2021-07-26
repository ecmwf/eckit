/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/ColumnExpression.h"

#include <cstring>
#include <ostream>

#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/ShiftedColumnExpression.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------


ColumnExpression::ColumnExpression(const std::string& name, const SQLTable* table, int begin, int end) :
    type_(0),
    value_(0),
    columnName_(name),
    table_(table),
    tableReference_(),
    beginIndex_(begin),
    endIndex_(end),
    nominalShift_(0) {}

ColumnExpression::ColumnExpression(const std::string& name, const std::string& tableReference, int begin, int end) :
    type_(0),
    value_(0),
    columnName_(name),
    table_(0),
    tableReference_(tableReference),
    beginIndex_(begin),
    endIndex_(end),
    nominalShift_(0) {}


std::shared_ptr<SQLExpression> ColumnExpression::clone() const {
    return std::make_shared<ColumnExpression>(*this);
}

std::shared_ptr<SQLExpression> ColumnExpression::reshift(int minColumnShift) const {
    return std::make_shared<ShiftedColumnExpression<ColumnExpression>>(*this, -minColumnShift, 0);
}

ColumnExpression::~ColumnExpression() {}

double ColumnExpression::eval(bool& missing) const {
    // n.b. we should only ever get here for numerical columns. Probably should ASSERT()
    if (value_->second)
        missing = true;
    return *(value_->first);
}

void ColumnExpression::eval(double* out, bool& missing) const {
    if (value_->second)
        missing = true;
    ::memcpy(out, value_->first, type_->size());
}

std::string ColumnExpression::evalAsString(bool& missing) const {
    if (value_->second)
        missing = true;
    return type_->asString(value_->first);
}

void ColumnExpression::preprepare(SQLSelect& sql) {

    /// pre-prepare exists to determine the Table/SQL column combination that is needed.
    ///
    /// This is then used in the SQLSelect to determine buffer sizes and data offsets, which
    /// can then be used to initialise value_ and name_ in the prepare() function.
    ///
    /// There is no straightforward way to do these both in one prepare() statement, without
    /// shifting the functionality outside.

    if (!table_)
        table_ = &sql.findTable(columnName_);
    sql.ensureFetch(*table_, columnName_);

    // Get the details into the ColumnExpression
    const SQLColumn& column(table_->column(columnName_));
    fullName_        = tableColumnToFullname(column);
    hasMissingValue_ = column.hasMissingValue();
    missingValue_    = column.missingValue();
    isBitfield_      = column.isBitfield();
    bitfieldDef_     = column.bitfieldDef();

    if (columnName_ == title() && columnName_ != fullName_) {
        title(fullName_);

        Log::debug() << "ColumnExpression::preprepare: columnName_=" << columnName_ << ", title[PATCHED]=" << title()
                     << std::endl;
    }
}

std::string ColumnExpression::tableColumnToFullname(const SQLColumn& column) const {
    return column.fullName();
}

void ColumnExpression::prepare(SQLSelect& sql) {
    updateType(sql);
}

void ColumnExpression::updateType(SQLSelect& sql) {

    // Get the memory address associated with reading from the column. (this is _not_
    // the SQLColumn object used in preprepare (which describe the columns requested
    // in the Select statement), but instead describes the mapping to the
    // SQLTableIterator object -- which describes what/how we are actually getting the
    // data.

    value_ = &sql.column(columnName_, table_);
    type_  = sql.typeOf(columnName_, table_);

    Log::debug() << "ColumnExpression::updateType: columnName_=" << columnName_ << ", title=" << title()
                 << ", table=" << table_->name() << ", fullName =" << fullName_ << " type=" << *type_
                 << " bitfieldDef.first.size =" << bitfieldDef_.first.size() << std::endl;
}

void ColumnExpression::cleanup(SQLSelect& sql) {
    value_ = 0;
    type_  = 0;
}


void ColumnExpression::print(std::ostream& s) const {
    s << columnName_;
    if (nominalShift_ != 0)
        s << "#" << nominalShift_;
    // if(table_) s << "@" << table_->fullName();
}

void ColumnExpression::output(SQLOutput& o) const {
    type_->output(o, value_->first, value_->second);
}

void ColumnExpression::expandStars(const std::vector<std::reference_wrapper<const SQLTable>>& tables,
                                   expression::Expressions& e) {
    std::ostream& L(Log::debug());
    L << "ColumnExpression::expandStars: expanding '" << columnName_ << "' (" << tableReference_ << ")" << std::endl;

    if (beginIndex_ != -1 && endIndex_ != -1) {
        ASSERT(beginIndex_ <= endIndex_);
        for (int i = beginIndex_; i <= endIndex_; i++)
            e.push_back(std::make_shared<ColumnExpression>(columnName_ + "_" + Translator<int, std::string>()(i),
                                                           this->table()));
        return;
    }

    if (columnName_ != "*") {
        // replace ColumnExpressions referring to aliases created with AS with references to appropriate expressions
        // If it's not an alias then:
        e.push_back(shared_from_this());
        return;
    }

    std::stringstream ss;

    unsigned int matched = 0;
    for (const SQLTable& table : tables) {
        std::vector<std::string> names = table.columnNames();

        for (size_t i = 0; i < names.size(); i++) {
            if ((tableReference_.size()) && ((names[i].rfind(tableReference_) == std::string::npos) || (names[i].rfind(tableReference_) + tableReference_.size() < names[i].size()))) {
                L << "ColumnExpression::expandStars: skip '" << names[i] << "'" << std::endl;
                continue;
            }

            ss << names[i] << ", ";
            ++matched;
            e.push_back(std::make_shared<ColumnExpression>(names[i], &table));
        }
    }
    if (!matched)
        throw eckit::UserError(std::string("No columns matching ") + columnName_ + tableReference_ + " found.");

    L << "ColumnExpression::expandStars: added " << ss.str() << std::endl;
}

void ColumnExpression::tables(std::set<const SQLTable*>& t) {
    ASSERT(table_);
    t.insert(table_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace sql
}  // namespace eckit
