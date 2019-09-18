/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLColumn.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLTable.h"

using namespace eckit;

namespace eckit {
namespace sql {

SQLColumn::SQLColumn(const type::SQLType& type, SQLTable& owner, const std::string& name, size_t index,
                     bool hasMissingValue, double missingValue, const BitfieldDef& bitfieldDef) :
    SQLIterator(type),
    noRows_(0),
    owner_(owner),
    name_(name),
    index_(index),
    current_(0),
    last_(0),
    position_(0),
    iterator_(0),
    hasMissingValue_(hasMissingValue),
    missingValue_(missingValue),
    isBitfield_(true),
    bitfieldDef_(bitfieldDef),
    sizeDoubles_(type.size() / 8) {
    ASSERT(type.size() % 8 == 0);
}

SQLColumn::SQLColumn(const SQLColumn& other) :
    SQLIterator(other.type()),
    noRows_(0),
    owner_(other.owner_),
    name_(other.name_),
    index_(other.index_),
    current_(0),
    last_(0),
    position_(0),
    iterator_(0),
    hasMissingValue_(other.hasMissingValue_),
    missingValue_(other.missingValue_),
    isBitfield_(other.isBitfield_),
    bitfieldDef_(other.bitfieldDef_),
    sizeDoubles_(other.sizeDoubles_) {
    ASSERT(type_.get().size() % 8 == 0);
}

SQLColumn::~SQLColumn() {}

void SQLColumn::rewind() {
    //	Timer timer("SQLColumn::rewind");
    rows_.clear();
    iterators_.clear();

    noRows_ = 0;
    // setPool(0);
}

void SQLColumn::setPool(int n) {

    if (iterator_)
        iterator_->unload();

    current_  = n;
    position_ = 0;
    last_     = rows_[n];
    iterator_ = iterators_[n];

    iterator_->rewind();

    // cout << "pool " << n << std::endl;
    // cout << "pool " << n << " " << last_ << std::endl;
}

double SQLColumn::next(bool& missing) {
    if (position_ == last_)
        setPool(current_ + 1);

    position_++;
    return iterator_->next(missing);
}

void SQLColumn::advance(unsigned long) {
    NOTIMP;
}

unsigned long long SQLColumn::noRows() const {
    if (!noRows_) {
        SQLColumn* col = const_cast<SQLColumn*>(this);
        col->rewind();
    }
    return noRows_;
}

void SQLColumn::print(std::ostream& s) const {}

std::string SQLColumn::fullName() const {
    // FIXME:
    return name();  // + "@" + owner_.fullName();
}

SQLTable* SQLColumn::table() const {
    return &owner_;
}


}  // namespace sql
}  // namespace eckit
