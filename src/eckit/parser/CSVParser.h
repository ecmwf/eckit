// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Feb 2019

#ifndef eckit_CSVParser_h
#define eckit_CSVParser_h

#include "eckit/parser/StreamParser.h"
#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class CSVParser : public StreamParser {

public:  // methods

    CSVParser(std::istream& in, bool hasHeader);
    Value parse();

    const Value& header() const;

    static Value decodeFile(const PathName& path, bool hasHeader);
    static Value decodeString(const std::string& str, bool hasHeader);

private:

    bool hasHeader_;
    Value header_;

    Value nextItem(bool&);
    ValueList nextLine();
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
