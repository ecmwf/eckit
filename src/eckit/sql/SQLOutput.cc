/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLOutput.h"

#include <ostream>

namespace eckit {
namespace sql {

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

}  // namespace sql
}  // namespace eckit
