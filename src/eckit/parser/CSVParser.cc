/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   CSVParser.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 2012

#include <fstream>

#include "eckit/value/Value.h"
#include "eckit/parser/CSVParser.h"
#include "eckit/utils/Translator.h"

namespace eckit {


CSVParser::CSVParser(std::istream &in, bool header):
    StreamParser(in, false),
    header_(header)
{
}

Value CSVParser::decodeFile(const PathName& path, bool header) {
    std::ifstream in(std::string(path).c_str());
    if (!in)
        throw eckit::CantOpenFile(path);
    return CSVParser(in, header).parse();
}

Value CSVParser::decodeString(const std::string& str, bool header) {
    std::istringstream in(str);
    return CSVParser(in, header).parse();
}

//----------------------------------------------------------------------------------------------------------------------

Value CSVParser::parse()
{
    if (header_) {
        ValueList header = nextLine();
        ValueList result;
        while (peek(true)) {
            ValueList line = nextLine();
            if (line.size() == 0) {
                break;
            }
            ValueMap m;
            ASSERT(header.size() == line.size());
            for (size_t i = 0; i < header.size(); ++i) {
                m[header[i]] = line[i];
            }
            result.push_back(m);
        }
        return result;
    }
    else {
        ValueList result;
        while (peek(true)) {
            ValueList line = nextLine();
            if (line.size() == 0) {
                break;
            }
            result.push_back(line);
        }
        return result;
    }
}

Value CSVParser::nextItem(bool& comma) {
    for (;;) {
        int c = peek(true);
        if (c != '\n' && c != '\r') {
            break;
        }
    }

    std::string result;
    comma = false;

    while (peek(true)) {
        int c = next(true);
        if (c == 0 || c == ',' || c == '\n' || c == '\r') {
            comma = (c == ',');
            return result;
        }
        result += char(c);
    }

    return result;
}

ValueList CSVParser::nextLine() {
    ValueList result;
    bool more = true;
    while (more) {
        auto x = nextItem(more);
        result.push_back(x);
    }
    return result;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
