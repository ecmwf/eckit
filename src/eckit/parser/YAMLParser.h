/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Jun 2012

#pragma once

#include <deque>

#include "eckit/parser/ObjectParser.h"
#include "eckit/parser/GenericYAMLParser.h"

namespace eckit {

using YAMLParser = GenericYAMLParser<parser::ValueBuilder, ObjectParser>;

}  // namespace eckit
