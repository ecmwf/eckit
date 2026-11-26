// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
