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

#ifndef eckit_JSONParser_h
#define eckit_JSONParser_h

#include "eckit/parser/ObjectParser.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class JSONParser : public ObjectParser {

public:  // methods

    JSONParser(std::istream& in);

    static Value decodeFile(const PathName& path);
    static Value decodeString(const std::string& str);

private:

    virtual Value parseValue();
    virtual std::string parserName() const;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
