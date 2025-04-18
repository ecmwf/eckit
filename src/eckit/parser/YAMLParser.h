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

#ifndef eckit_YAMLParser_h
#define eckit_YAMLParser_h

#include <deque>

#include "eckit/parser/ObjectParser.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


struct YAMLItem;

class YAMLParser : public ObjectParser {


public:  // methods

    YAMLParser(std::istream& in);
    ~YAMLParser() override;

    static Value decodeFile(const PathName& path);
    static Value decodeString(const std::string& str);

private:

    std::deque<YAMLItem*> items_;
    YAMLItem* last_;

    std::vector<char> stop_;
    std::vector<char> comma_;
    std::vector<char> colon_;

    std::map<Value, Value> anchors_;

private:

    void loadItem();
    const YAMLItem& nextItem();
    const YAMLItem& peekItem();

    std::string nextWord();
    size_t consumeChars(char);
    Value consumeJSON(char);


    std::string nextToken(size_t& spaces, size_t& lines);

    bool endOfToken(char);

    Value parseValue() override;

    Value parseString(char quote = '"') override;
    Value parseNumber() override;

    Value parseStringOrNumber(bool& isKey);

    std::string parserName() const override;


    void anchor(const Value& key, const Value& value);
    Value anchor(const Value& key) const;

    friend struct YAMLItemKey;
    friend struct YAMLItemEntry;
    friend struct YAMLItemStartDocument;
    friend struct YAMLItemEndDocument;
    friend struct YAMLItemAnchor;
    friend struct YAMLItemReference;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
