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
