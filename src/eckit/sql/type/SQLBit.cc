/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/LibEcKitSQL.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/type/SQLBit.h"

namespace eckit {
namespace sql {
namespace type {

SQLBit::SQLBit(const std::string& name, unsigned long mask, unsigned long shift) :
    type::SQLType(name),
    returnAsDouble_(LibEcKitSQL::instance().treatIntegersAsDoubles()),
    mask_(mask),
    shift_(shift) {}

SQLBit::~SQLBit() {}

// This is an odd one, but it allows us to store values in DISTINCT/ORDER expressions
size_t SQLBit::size() const {
    return sizeof(long);
}

void SQLBit::output(SQLOutput& o, double x, bool missing) const {

    unsigned long val = returnAsDouble_ ? static_cast<unsigned long>(x)
                                        : reinterpret_cast<const int64_t&>(x);

    val = (val & mask_) >> shift_;

    double retval = missing ? 0
                            : (returnAsDouble_ ? static_cast<double>(val)
                                               : reinterpret_cast<const double&>(val));
    o.outputUnsignedInt(retval, missing);
}

std::string SQLBit::asString(const double* val) const {
    return ((decltype(mask_)(*val) & mask_) == 0) ? "0" : "1";
}

}  // namespace type
}  // namespace sql
}  // namespace eckit
