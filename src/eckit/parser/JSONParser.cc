// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   JSONParser.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 2012

#include "eckit/parser/JSONParser.h"

#include <fstream>

#include "eckit/utils/Translator.h"
#include "eckit/value/Value.h"

namespace eckit {


JSONParser::JSONParser(std::istream& in) : ObjectParser(in, false, false) {}

Value JSONParser::decodeFile(const PathName& path) {
    std::ifstream in(std::string(path).c_str());
    if (!in) {
        throw eckit::CantOpenFile(path);
    }
    return JSONParser(in).parse();
}

Value JSONParser::decodeString(const std::string& str) {
    std::istringstream in(str);
    return JSONParser(in).parse();
}

//----------------------------------------------------------------------------------------------------------------------

Value JSONParser::parseValue() {
    return parseJSON();
}

std::string JSONParser::parserName() const {
    return "JSONParser";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
