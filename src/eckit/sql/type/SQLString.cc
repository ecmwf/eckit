// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/type/SQLString.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit::sql::type {

SQLString::SQLString(const std::string& name, size_t maxLen) : SQLType(name), maxLen_(maxLen) {
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
        o.outputString(nullptr, 0, missing);
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

}  // namespace eckit::sql::type
