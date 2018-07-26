/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include <sstream>

#include "eckit/eckit.h"
#include "eckit/utils/Translator.h"
#include "eckit/sql/expression//ColumnExpression.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------


static double dzero = 0;
static bool mzero = false;
static std::pair<double*,bool&> zero_(&dzero, mzero);

ColumnExpression::ColumnExpression(const std::string& name, SQLTable* table, int begin, int end)
: type_(0),
  value_(zero_),
  columnName_(name),
  table_(table),
  tableReference_(),
  beginIndex_(begin),
  endIndex_(end),
  nominalShift_(0)
{}

ColumnExpression::ColumnExpression(const std::string& name, const std::string& tableReference, int begin, int end)
: type_(0),
  value_(zero_),
  columnName_(name),
  table_(0),
  tableReference_(tableReference),
  beginIndex_(begin),
  endIndex_(end),
  nominalShift_(0)
{}

ColumnExpression::ColumnExpression(const ColumnExpression& e)
: type_(e.type_),
  value_(e.value_),
  columnName_(e.columnName_),
  table_(e.table_),
  tableReference_(e.tableReference_),
  beginIndex_(e.beginIndex_),
  endIndex_(e.endIndex_),
  nominalShift_(e.nominalShift_)
{}

std::shared_ptr<SQLExpression> ColumnExpression::clone() const { return std::make_shared<ColumnExpression>(*this); }

ColumnExpression::~ColumnExpression() {}

double ColumnExpression::eval(bool& missing) const
{
    if (value_.second) missing = true;
    return *value_.first;
}

void ColumnExpression::prepare(SQLSelect& sql)
{
	std::string fullName;

    SQLTable& table = sql.findTable(columnName_, &fullName, &hasMissingValue_, &missingValue_, &isBitfield_, &bitfieldDef_);

    if(!table_) table_ = &table;
 
	value_ = sql.column(columnName_, table_);
	type_  = sql.typeOf(columnName_, table_);

	Log::debug() << "ColumnExpression::prepare: columnName_=" << columnName_ 
	<< ", title=" << title()
	<< ", table=" << table_->name()
	<< ", fullName =" << fullName
	<< " type=" << *type_ 
	<< " bitfieldDef.first.size =" << bitfieldDef_.first.size()
	<< std::endl;

	if (columnName_ == title() && columnName_ != fullName)
	{
		title(fullName);

		Log::debug() << "ColumnExpression::prepare: columnName_=" << columnName_ 
		<< ", title[PATCHED]=" << title()
		<< std::endl;
	}
}

void ColumnExpression::cleanup(SQLSelect& sql)
{
    value_ = zero_;
	type_  = 0;
}


void ColumnExpression::print(std::ostream& s) const 
{
	s << columnName_;
	if (nominalShift_ != 0)
		s << "#" << nominalShift_;
	//if(table_) s << "@" << table_->fullName();
}

void ColumnExpression::output(SQLOutput& o) const 
{ 
	bool missing = false;
	double v = eval(missing);
	type_->output(o, v, missing); 
}

void ColumnExpression::expandStars(const std::vector<std::reference_wrapper<SQLTable>>& tables, expression::Expressions& e)
{
    std::ostream& L(Log::debug());
	L << "ColumnExpression::expandStars: expanding '" << columnName_ << "' (" << tableReference_ << ")" << std::endl;

	if(beginIndex_ != -1 && endIndex_ != -1)
	{
		ASSERT(beginIndex_ <= endIndex_);
		for(int i = beginIndex_; i <= endIndex_; i++)
            e.push_back(std::make_shared<ColumnExpression>(columnName_ + "_" + Translator<int,std::string>()(i), this->table()));
		return;
	}

	if(columnName_ != "*")
	{
        // replace ColumnExpressions referring to aliases created with AS with references to appropriate expressions 
        // If it's not an alias then:
        e.push_back(shared_from_this());
		return;
	}

    std::stringstream ss;
	
	unsigned int matched = 0;
    for (SQLTable& table : tables) {
        std::cout << "TABLE" << std::endl;

        std::vector<std::string> names = table.columnNames();

		for(size_t i = 0; i < names.size(); i++)
		{
            std::cout << "Name: " << names[i] << std::endl;
			if ((tableReference_.size())
				&& ((names[i].rfind(tableReference_) == std::string::npos)
					|| (names[i].rfind(tableReference_) + tableReference_.size() < names[i].size())))
			{
				L << "ColumnExpression::expandStars: skip '" << names[i] << "'" << std::endl;
				continue;
			}
			
			ss << names[i] << ", ";
			++matched;
            e.push_back(std::make_shared<ColumnExpression>(names[i], &table));
		}
	}
	if (! matched)
		throw eckit::UserError(std::string("No columns matching ") + columnName_ + tableReference_ + " found.");

	L << "ColumnExpression::expandStars: added " << ss.str() << std::endl;
}

void ColumnExpression::tables(std::set<SQLTable*>& t) 
{ 
	ASSERT(table_);
	t.insert(table_); 
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace expression
} // namespace sql
} // namespace odb

