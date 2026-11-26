// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/sql/type/SQLInt.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/utils/Translator.h"

namespace eckit::sql::type {

//----------------------------------------------------------------------------------------------------------------------

SQLInt::SQLInt(const std::string& name) : SQLType(name) {}

SQLInt::~SQLInt() {}

size_t SQLInt::size() const {
    return sizeof(long);
}

void SQLInt::output(SQLOutput& o, double d, bool missing) const {
    o.outputInt(d, missing);
}

std::string SQLInt::asString(const double* val) const {
    return eckit::Translator<long, std::string>()(*val);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::type
