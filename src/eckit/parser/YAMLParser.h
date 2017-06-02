/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Jun 2012

#ifndef eckit_YAMLParser_h
#define eckit_YAMLParser_h

#include <deque>

#include "eckit/parser/ObjectParser.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


struct YAMLItem;

class YAMLParser : public ObjectParser {



public: // methods

    YAMLParser(std::istream & in);
    virtual ~YAMLParser();

    static Value decodeFile(const PathName & path);
    static Value decodeString(const std::string & str);

private:

    std::deque<YAMLItem*> items_;
    YAMLItem* last_;

    std::vector<char> stop_;
    std::vector<char> comma_;

private:

    void loadItem();
    const YAMLItem& nextItem();
    const YAMLItem& peekItem();

    virtual Value parseValue();
    virtual Value parseString();


    void pushIndent();
    void popIndent();

    friend struct YAMLItemKey;
    friend struct YAMLItemEntry;

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
