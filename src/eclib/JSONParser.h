/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file JSONParser.h
/// @author Baudouin Raoult
/// @date Jun 2012

#ifndef eclib_JSONParser_H
#define eclib_JSONParser_H

#include "eclib/StreamParser.h"
#include "eclib/Value.h"

class JSONParser : public StreamParser {

public: // methods

    JSONParser(istream& in);
    
    Value parse();

private: // methods

    Value parseTrue();
    Value parseFalse();
    Value parseNull();
    Value parseValue();
    Value parseObject();
    Value parseArray();
    Value parseString();
    Value parseNumber();
    void parseKeyValue(map<Value, Value> &);

};

#endif
