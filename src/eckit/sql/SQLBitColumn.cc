/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLBitColumn.h"
#include "eckit/eckit.h"
#include "eckit/log/Log.h"
#include "eckit/sql/type/SQLBitfield.h"

using namespace eckit;

namespace eckit {
namespace sql {

SQLBitColumn::SQLBitColumn(const SQLColumn& column, const std::string& field) :
    SQLColumn(column), field_(field), mask_(0), shift_(0) {
    const type::SQLBitfield& t = dynamic_cast<const type::SQLBitfield&>(type());
    mask_                      = t.mask(field);
    shift_                     = t.shift(field);

    Log::info() << "here " << field << " mask=" << std::hex << mask_ << std::dec << " shift=" << shift_ << std::endl;
}

SQLBitColumn::~SQLBitColumn() {}

void SQLBitColumn::rewind() {
    SQLColumn::rewind();
}

double SQLBitColumn::next(bool& missing) {
    Log::info() << "SQLBitColumn::next: " << std::endl;

    unsigned long value = static_cast<unsigned long>(SQLColumn::next(missing));
    return (value >> shift_) & mask_;
}

void SQLBitColumn::advance(unsigned long n) {
    SQLColumn::advance(n);
}

void SQLBitColumn::print(std::ostream& s) const {}

}  // namespace sql
}  // namespace eckit
