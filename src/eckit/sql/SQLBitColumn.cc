// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/SQLBitColumn.h"

#include "eckit/eckit.h"
#include "eckit/log/Log.h"
#include "eckit/sql/type/SQLBitfield.h"

using namespace eckit;

namespace eckit::sql {

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

}  // namespace eckit::sql
