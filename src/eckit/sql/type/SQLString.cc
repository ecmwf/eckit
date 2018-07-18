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
#include "eckit/sql/type/SQLString.h"

namespace eckit {
namespace sql {
namespace type {

SQLString::SQLString(const std::string& name) : SQLType(name) {} 

SQLString::~SQLString() {}

size_t SQLString::size() const { return sizeof(double); }

void SQLString::output(SQLOutput& o, double d, bool missing) const { o.outputString(d, missing); }

SQLType::manipulator SQLString::format() const { return &std::left; }

size_t SQLString::width() const { return 10; }

} // namespace type 
} // namespace sql
} // namespace eckit
