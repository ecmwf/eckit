/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/type/SQLString.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {
namespace type {

SQLString::SQLString(const std::string& name, size_t maxLen) :
    SQLType(name), maxLen_(maxLen) {
    ASSERT(maxLen_ % 8 == 0);
}

SQLString::~SQLString() {}

size_t SQLString::size() const {
    return maxLen_;
}

void SQLString::output(SQLOutput& o, double d, bool missing) const {
    throw SeriousBug("We should never hit this override", Here());
}

void SQLString::output(SQLOutput& o, const double* d, bool missing) const {
    if (missing) {
        o.outputString(0, 0, missing);
    }
    else {
        const char* c(reinterpret_cast<const char*>(d));
        o.outputString(c, ::strnlen(c, maxLen_), missing);
    }
}

std::string SQLString::asString(const double* val) const {
    const char* c(reinterpret_cast<const char*>(val));
    return std::string(c, ::strnlen(c, maxLen_));
}

SQLType::manipulator SQLString::format() const {
    return &std::left;
}

size_t SQLString::width() const {
    return maxLen_ + 2;
}

}  // namespace type
}  // namespace sql
}  // namespace eckit
