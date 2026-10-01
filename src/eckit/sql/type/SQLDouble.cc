// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/type/SQLDouble.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/utils/Translator.h"

namespace eckit::sql::type {

//----------------------------------------------------------------------------------------------------------------------

SQLDouble::SQLDouble(const std::string& name) : SQLType(name) {}

SQLDouble::~SQLDouble() {}

size_t SQLDouble::size() const {
    return sizeof(double);
}

void SQLDouble::output(SQLOutput& o, double d, bool m) const {
    o.outputDouble(d, m);
}

std::string SQLDouble::asString(const double* val) const {
    return eckit::Translator<double, std::string>()(*val);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::type
