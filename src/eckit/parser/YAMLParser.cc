/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   YAMLParser.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 2012

#include "eckit/value/Value.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/utils/Translator.h"

namespace eckit {


YAMLParser::YAMLParser(std::istream &in):
    ObjectParser(in, true)
{
}

Value YAMLParser::decodeFile(const PathName& path) {
    return ObjectParser::decodeFile(path, true);
}

Value YAMLParser::decodeString(const std::string& str) {
    return ObjectParser::decodeString(str, true);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
