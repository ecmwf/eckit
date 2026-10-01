// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/type/SQLReal.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/utils/Translator.h"

namespace eckit::sql::type {

//----------------------------------------------------------------------------------------------------------------------

SQLReal::SQLReal(const std::string& name) : SQLType(name) {}

SQLReal::~SQLReal() {}

size_t SQLReal::size() const {
    return sizeof(double);
}

void SQLReal::output(SQLOutput& o, double d, bool m) const {
    o.outputReal(d, m);
}

std::string SQLReal::asString(const double* val) const {
    return eckit::Translator<double, std::string>()(*val);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::type
