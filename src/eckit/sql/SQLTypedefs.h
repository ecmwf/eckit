/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_sql_SQLTypes_h
#define eckit_sql_SQLTypes_h

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace eckit::sql {

typedef std::vector<std::string> FieldNames;
typedef std::vector<int32_t> Sizes;
typedef std::pair<FieldNames, Sizes> BitfieldDef;
typedef std::map<std::string, BitfieldDef> BitfieldDefs;

}  // namespace eckit::sql

#endif
