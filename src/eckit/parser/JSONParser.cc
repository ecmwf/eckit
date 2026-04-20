/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   JSONParser.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 2012

#include <fstream>

#include "eckit/parser/JSONParser.h"
#include "eckit/utils/Translator.h"
#include "eckit/value/Value.h"

#include "eckit/eckit.h"

#if eckit_HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif


namespace eckit {

#if eckit_HAVE_NLOHMANN_JSON

//----------------------------------------------------------------------------------------------------------------------
// nlohmann/json-based implementation
//----------------------------------------------------------------------------------------------------------------------

namespace {

Value nlohmannToValue(const nlohmann::json& j) {
    if (j.is_null()) {
        return Value();
    }
    if (j.is_boolean()) {
        return Value(j.get<bool>());
    }
    if (j.is_number_integer()) {
        return Value(j.get<long long>());
    }
    if (j.is_number_unsigned()) {
        return Value(static_cast<long long>(j.get<unsigned long long>()));
    }
    if (j.is_number_float()) {
        return Value(j.get<double>());
    }
    if (j.is_string()) {
        return Value(j.get<std::string>());
    }
    if (j.is_array()) {
        ValueList list;
        for (const auto& elem : j) {
            list.push_back(nlohmannToValue(elem));
        }
        return Value::makeList(list);
    }
    if (j.is_object()) {
        ValueMap map;
        for (auto it = j.begin(); it != j.end(); ++it) {
            map[it.key()] = nlohmannToValue(it.value());
        }
        return Value::makeMap(map);
    }
    throw SeriousBug("Unknown JSON type");
}

}  // namespace


JSONParser::JSONParser(std::istream& in) : ObjectParser(in, false, false) {}

Value JSONParser::decodeFile(const PathName& path) {
    std::ifstream in(std::string(path).c_str());
    if (!in) {
        throw eckit::CantOpenFile(path);
    }
    nlohmann::json j = nlohmann::json::parse(in);
    return nlohmannToValue(j);
}

Value JSONParser::decodeString(const std::string& str) {
    nlohmann::json j = nlohmann::json::parse(str);
    return nlohmannToValue(j);
}

Value JSONParser::parseValue() {
    return parseJSON();
}

std::string JSONParser::parserName() const {
    return "JSONParser (nlohmann/json)";
}

#else

//----------------------------------------------------------------------------------------------------------------------
// Built-in implementation
//----------------------------------------------------------------------------------------------------------------------

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

#endif

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
