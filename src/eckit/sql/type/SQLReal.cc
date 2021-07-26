/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/type/SQLReal.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace sql {
namespace type {

//----------------------------------------------------------------------------------------------------------------------

SQLReal::SQLReal(const std::string& name) :
    SQLType(name) {}

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

}  // namespace type
}  // namespace sql
}  // namespace eckit
