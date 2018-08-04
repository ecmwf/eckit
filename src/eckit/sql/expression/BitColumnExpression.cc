/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>

#include "eckit/eckit.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/sql/expression//BitColumnExpression.h"
#include "eckit/sql/type/SQLBit.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

BitColumnExpression::BitColumnExpression(const std::string& name, const std::string& field, SQLTable* table)
: ColumnExpression(name + "." + field + "@" + table->name(), table),
  mask_(0),
  bitShift_(0),
  field_(field),
  name_(name)
{
	Log::debug() << "BitColumnExpression::BitColumnExpression: name=" << name
		<< ", field=" << field << ", table->name() =" << table->name()
		<< ": name_=" << name_
		<< std::endl;
}

BitColumnExpression::BitColumnExpression(const std::string& name, const std::string& field, const std::string& tableReference)
: ColumnExpression(name + "." + field + tableReference, tableReference),
  mask_(0),
  bitShift_(0),
  field_(field),
  name_(name)
{
	Log::debug() << "BitColumnExpression::BitColumnExpression: name=" << name
		<< ", field=" << field << ", tableReference=" << tableReference
		<< ": name_=" << name_
		<< std::endl;
}

BitColumnExpression::BitColumnExpression(const BitColumnExpression& o)
: ColumnExpression(o),
  mask_(o.mask_),
  bitShift_(o.bitShift_),
  field_(o.field_),
  name_(o.name_)
{}

BitColumnExpression::~BitColumnExpression() {}

const eckit::sql::type::SQLType* BitColumnExpression::type() const
{
    // Change the type to integer to be able to create a new ODA if necessary
    return &eckit::sql::type::SQLType::lookup("integer");
}

void BitColumnExpression::prepare(SQLSelect& sql)
{
	std::string name = name_ + "." + field_ + tableReference_;
	if(!table_)
        table_ = &sql.findTable(name);
    value_ = &sql.column(name, table_);
	type_  = sql.typeOf(name, table_);


	const type::SQLBit* bit = dynamic_cast<const type::SQLBit*>(type_);
	if(bit)
	{
		mask_  = bit->mask();
		bitShift_ = bit->shift();
	}
	else
	{
		// This is for .length and .offset
		// Not very nice, I know
		mask_  = 0xffffffff;
		bitShift_ = 0;
	}

}

double BitColumnExpression::eval(bool& missing) const
{
    if (value_->second) missing = true;
    unsigned long x = static_cast<unsigned long>(*value_->first);
	return (x & mask_) >> bitShift_;
}

void BitColumnExpression::expandStars(const std::vector<std::reference_wrapper<SQLTable>>& tables, expression::Expressions& e)
{
    using namespace eckit;
    using namespace std;

    Log::debug() << "BitColumnExpression::expandStars: " << e << endl;
	// TODO: regex
	if(field_ != "*")
	{
        NOTIMP;
//		e.push_back(this);
		return;
	}

    for (const SQLTable& table : tables) {

        std::vector<std::string> names = table.bitColumnNames(name_ + tableReference_);

		for(size_t i = 0; i < names.size(); i++)
		{
            e.push_back(std::make_shared<BitColumnExpression>(name_, names[i], tableReference_ /*table*/));
		}
	}

	delete this;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace expression
} // namespace sql
} // namespace odb

