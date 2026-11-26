// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/type/SQLBit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit::sql::type {

SQLBit::SQLBit(const std::string& name, unsigned long mask, unsigned long shift) :
    type::SQLType(name), mask_(mask), shift_(shift) {}

SQLBit::~SQLBit() {}

// This is an odd one, but it allows us to store values in DISTINCT/ORDER expressions
size_t SQLBit::size() const {
    return sizeof(long);
}

void SQLBit::output(SQLOutput& o, double x, bool missing) const {
    double val = (missing ? 0 : ((static_cast<unsigned long>(x) & mask_) >> shift_));
    o.outputUnsignedInt(val, missing);
}

std::string SQLBit::asString(const double* val) const {
    return ((decltype(mask_)(*val) & mask_) == 0) ? "0" : "1";
}

}  // namespace eckit::sql::type
