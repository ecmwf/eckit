/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/BitColumnExpression.h"

#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/os/BackTrace.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/ShiftedColumnExpression.h"
#include "eckit/sql/type/SQLBit.h"
#include "eckit/utils/Tokenizer.h"

// Cray C++ compiler should not try to optimize this code
#if _CRAYC
#pragma _CRI noopt
#endif


namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

BitColumnExpression::BitColumnExpression(const std::string& name, const std::string& field, SQLTable* table) :
    ColumnExpression(name + "." + field + "@" + table->name(), table),
    mask_(0),
    bitShift_(0),
    field_(field),
    name_(name) {
    Log::debug() << "BitColumnExpression::BitColumnExpression: name=" << name << ", field=" << field
                 << ", table->name() =" << table->name() << ": name_=" << name_ << std::endl;
}

BitColumnExpression::BitColumnExpression(const std::string& name, const std::string& field,
                                         const std::string& tableReference) :
    ColumnExpression(name + "." + field + tableReference, tableReference),
    mask_(0),
    bitShift_(0),
    field_(field),
    name_(name) {
    Log::debug() << "BitColumnExpression::BitColumnExpression: name=" << name << ", field=" << field
                 << ", tableReference=" << tableReference << ": name_=" << name_ << std::endl;
}

BitColumnExpression::BitColumnExpression(const BitColumnExpression& o) :
    ColumnExpression(o), mask_(o.mask_), bitShift_(o.bitShift_), field_(o.field_), name_(o.name_) {}

BitColumnExpression::~BitColumnExpression() {}

const eckit::sql::type::SQLType* BitColumnExpression::type() const {
    // Change the type to integer to be able to create a new ODA if necessary
    return &eckit::sql::type::SQLType::lookup("integer");
}

void BitColumnExpression::prepare(SQLSelect& sql) {
    updateType(sql);
}

void BitColumnExpression::updateType(SQLSelect& sql) {
    std::string name = name_ + "." + field_ + tableReference_;
    if (!table_)
        table_ = &sql.findTable(name);
    value_ = &sql.column(name, table_);
    type_  = sql.typeOf(name, table_);


    const type::SQLBit* bit = dynamic_cast<const type::SQLBit*>(type_);
    if (bit) {
        mask_     = bit->mask();
        bitShift_ = bit->shift();
    }
    else {
        // This is for .length and .offset
        // Not very nice, I know
        mask_     = 0xffffffff;
        bitShift_ = 0;
    }
}

double BitColumnExpression::eval(bool& missing) const {
    if (value_->second)
        missing = true;
    unsigned long x = static_cast<unsigned long>(*value_->first);
    return (x & mask_) >> bitShift_;
}

void BitColumnExpression::expandStars(const std::vector<std::reference_wrapper<const SQLTable>>& tables,
                                      expression::Expressions& e) {
    using namespace eckit;
    using namespace std;

    // TODO: regex
    if (field_ != "*") {
        e.push_back(shared_from_this());
        return;
    }

    for (const SQLTable& table : tables) {

        std::vector<std::string> names = table.bitColumnNames(name_ + tableReference_);

        for (size_t i = 0; i < names.size(); i++) {
            e.push_back(std::make_shared<BitColumnExpression>(name_, names[i], tableReference_ /*table*/));
        }
    }
}

std::shared_ptr<SQLExpression> BitColumnExpression::clone() const {
    return std::make_shared<BitColumnExpression>(*this);
}

std::shared_ptr<SQLExpression> BitColumnExpression::reshift(int minColumnShift) const {
    return std::make_shared<ShiftedColumnExpression<BitColumnExpression>>(*this, -minColumnShift, 0);
}

std::string BitColumnExpression::tableColumnToFullname(const SQLColumn& column) const {
    // The fully expanded table column will not include the field name, as we are extracting
    // the specified bits out of that column.
    //
    // This means that to construct the fully qualified field name, we need to include
    // these field names

    std::vector<std::string> bits;
    Tokenizer('@')(column.fullName(), bits);

    ASSERT(bits.size() > 0);
    ASSERT(bits.size() < 3);

    if (bits[0] != name_ + "." + field_) {
        bits[0] += "." + field_;
    }
    if (bits.size() == 2) {
        return bits[0] + "@" + bits[1];
    }
    return bits[0];
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace sql
}  // namespace eckit
