/*
 * (C) Copyright 1996-2013 ECMWF.
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

#include "eckit/parser/StreamParser.h"
#include "eckit/types/Types.h"
#include "eckit/value/Value.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class JSONParser : public StreamParser {

public: // methods

    JSONParser(std::istream& in);
    
    Value parse();
    
    static void toStrDict(Value&, StringDict& );

private: // methods

    Value parseTrue();
    Value parseFalse();
    Value parseNull();
    Value parseValue();
    Value parseObject();
    Value parseArray();
    Value parseString();
    Value parseNumber();
    
    void parseKeyValue(std::map<Value, Value> &);

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
