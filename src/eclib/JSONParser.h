/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JSON.h
// Baudouin Raoult - (c) ECMWF Jun 12

#include "eclib/machine.h"
#include "eclib/Value.h"

class JSONParser {

public:
    JSONParser(istream& in) : in_(in) {}
    Value parse();

private:
    istream& in_;


    char peek();
    char next(bool spaces = false);

    void consume(char);
    void consume(const char*);

    void expect(const char*);

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
