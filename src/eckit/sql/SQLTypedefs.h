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

using FieldNames   = std::vector<std::string>;
using Sizes        = std::vector<int32_t>;
using BitfieldDef  = std::pair<FieldNames, Sizes>;
using BitfieldDefs = std::map<std::string, BitfieldDef>;

}  // namespace eckit::sql

#endif
