/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/sql/type/SQLInt.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace sql {
namespace type {

//----------------------------------------------------------------------------------------------------------------------

SQLInt::SQLInt(const std::string& name) :
    SQLType(name) {}

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

}  // namespace type
}  // namespace sql
}  // namespace eckit
