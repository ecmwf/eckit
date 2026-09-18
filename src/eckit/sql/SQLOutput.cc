// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/SQLOutput.h"

#include <ostream>

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

SQLOutput::SQLOutput() {}

SQLOutput::~SQLOutput() {}

void SQLOutput::preprepare(SQLSelect&) {}

bool SQLOutput::cachedNext() {
    return false;
}

void SQLOutput::print(std::ostream& s) const {
    s << "SQLOutput" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql
