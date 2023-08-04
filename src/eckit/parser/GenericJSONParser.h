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

#include <fstream>

#include "eckit/parser/GenericObjectParser.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder, typename ObjectParserBase = GenericObjectParser<ObjectBuilder> >
class GenericJSONParser : public ObjectParserBase {

public:  // methods
    using Traits = typename ObjectParserBase::Traits;

    GenericJSONParser(std::istream& in) :
        ObjectParserBase(in, false, false){};

    static typename Traits::ValueType decodeFile(const PathName& path) {
        std::ifstream in(std::string(path).c_str());
        if (!in) {
            throw eckit::CantOpenFile(path);
        }
        return GenericJSONParser<ObjectBuilder, ObjectParserBase>(in).parse();
    }

    static typename Traits::ValueType decodeString(const std::string& str) {
        std::istringstream in(str);
        return GenericJSONParser<ObjectBuilder, ObjectParserBase>(in).parse();
    }

private:
    virtual typename Traits::ValueType parseValue() {
        return this->parseJSON();
    }

    virtual std::string parserName() const {
        return "JSONParser";
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
