// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
