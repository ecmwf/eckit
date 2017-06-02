/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   YAMLParser.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 2012

#include "eckit/value/Value.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/utils/Translator.h"
#include "eckit/memory/Counted.h"

namespace eckit {


struct YAMLItem : public Counted {

    long indent_;
    Value value_;


    virtual void print(std::ostream& s) const = 0;
    virtual Value parse(YAMLParser& parser) const {
        return value_;
    }

    YAMLItem(long indent = 0, const Value& value = Value()):
        indent_(indent),
        value_(value) {}

    virtual ~YAMLItem() {};

    friend std::ostream& operator<<(std::ostream& s, const YAMLItem& item)
    { item.print(s); return s;}
};


struct YAMLItemEOF : public YAMLItem {

    virtual void print(std::ostream& s) const {
        s << "YAMLItemEOF";
    }


    YAMLItemEOF(): YAMLItem(-1) {}


};


struct YAMLItemValue : public YAMLItem {

    virtual void print(std::ostream& s) const {
        s << "YAMLItemValue[value=" << value_ << ", indent=" << indent_ << "]";
    }

    YAMLItemValue(size_t indent, const Value& value): YAMLItem(indent, value) {}

};

struct YAMLItemKey : public YAMLItem {

    virtual void print(std::ostream& s) const {
        s << "YAMLItemKey[value=" << value_ << ", indent=" << indent_ << "]";
    }

    YAMLItemKey(const YAMLItem& item): YAMLItem(item.indent_, item.value_) {}

    Value parse(YAMLParser& parser) const {
        std::map<Value, Value> m;


        const YAMLItem* key = this;

        bool more = true;
        while (more) {


            const YAMLItem& next = parser.peekItem();

            if (next.indent_ == key->indent_) {
                // Special case
                m[key->value_] = Value(); // null
                key = &parser.nextItem();
                ASSERT(dynamic_cast<const YAMLItemKey*>(key));
                continue;
            }

            if (next.indent_ < key->indent_) {
                // Special case
                m[key->value_] = Value(); // null
                more = false;
                continue;
            }

            if (next.indent_ > key->indent_) {
                m[key->value_] = parser.nextItem().parse(parser);
            }

            const YAMLItem& peek = parser.peekItem();

            if (peek.indent_ < key->indent_) {
                more = false;
                continue;
            }

            if (peek.indent_ == key->indent_) {
                key = &parser.nextItem();
                ASSERT(dynamic_cast<const YAMLItemKey*>(key));
                continue;
            }

            ASSERT(false);

        }

        return Value::makeMap(m);

    }

};



struct YAMLItemEntry : public YAMLItem {

    virtual void print(std::ostream& s) const {
        s << "YAMLItemEntry[indent=" << indent_ << "]";
    }

    YAMLItemEntry(size_t indent): YAMLItem(indent) {}

    Value parse(YAMLParser& parser) const {
        std::vector<Value> l;

        bool more = true;
        while (more) {

            const YAMLItem& next = parser.peekItem();

            if (next.indent_ == indent_) {
                // Special case
                l.push_back(Value()); // null
                const YAMLItem* advance = &parser.nextItem();
                ASSERT(dynamic_cast<const YAMLItemEntry*>(advance));
                continue;
            }

            if (next.indent_ < indent_) {
                // Special case
                l.push_back(Value()); // null
                more = false;
                continue;
            }

            if (next.indent_ > indent_) {
                l.push_back(parser.nextItem().parse(parser));
            }

            const YAMLItem& peek = parser.peekItem();

            if (peek.indent_ < indent_) {
                more = false;
                continue;
            }

            if (peek.indent_ == indent_) {
                const YAMLItem* advance = &parser.nextItem();
                ASSERT(dynamic_cast<const YAMLItemEntry*>(advance));
                continue;
            }

            ASSERT(false);

        }

        return Value::makeList(l);

    }

};

YAMLParser::YAMLParser(std::istream &in):
    ObjectParser(in, true, false) {
    stop_.push_back(0);
    comma_.push_back(0);
}

YAMLParser::~YAMLParser() {
    for (std::deque<YAMLItem*>::iterator j = items_.begin(); j != items_.end(); ++j) {
        (*j)->detach();
    }
}

Value YAMLParser::decodeFile(const PathName& path) {
    std::ifstream in(std::string(path).c_str());
    if (!in)
        throw eckit::CantOpenFile(path);
    return YAMLParser(in).parse();
}

Value YAMLParser::decodeString(const std::string& str) {
    std::istringstream in(str);
    return YAMLParser(in).parse();
}


Value YAMLParser::parseString() {


    char c = peek();

    if (c == '\"') {
        return ObjectParser::parseString();
    }

    std::string s;
    size_t last = 0;
    size_t i = 0;

    while (c != ':' && c != '\n' && c != 0 && c != stop_.back() && c != comma_.back()) {
        char p = next(true);
        s += p;
        if (!::isspace(p)) {
            last = i;
        }
        c = peek(true);
        i++;
    }

    s = s.substr(0, last + 1);

    if (s == "null") {
        return Value();
    }

    if (s == "false") {
        return Value(false);
    }

    if (s == "true") {
        return Value(true);
    }

    return Value("|" + s + "|");
}

void YAMLParser::loadItem()
{

    if (!items_.empty()) {
        return;
    }

    char c = peek();

    size_t indent = pos_;

    YAMLItem* item = 0;


    switch (c)
    {

    case 0:
        item = new YAMLItemEOF();
        break;

    case '{':
        stop_.push_back('}');
        comma_.push_back(',');
        item = new YAMLItemValue(indent, parseJSON());
        stop_.pop_back();
        comma_.pop_back();
        break;

    case '[':
        stop_.push_back(']');
        comma_.push_back(',');
        item = new YAMLItemValue(indent, parseJSON());
        stop_.pop_back();
        comma_.pop_back();
        break;

    case '\"':
        item = new YAMLItemValue(indent, parseString());
        break;

    case '-':
        consume('-');
        c = peek(true);
        if (::isspace(c)) {
            item = new YAMLItemEntry(indent);
        } else {
            if (::isdigit(c)) {
                item = new YAMLItemValue(indent, -parseNumber());
            }
            else {
                item = new YAMLItemValue(indent, Value("-") + parseString());
            }
        }
        break;


    default:
        if (::isdigit(c)) {
            item = new YAMLItemValue(indent, parseNumber());
        }
        else {
            item = new YAMLItemValue(indent, parseString());

        }
        break;

    }

    ASSERT(item);

    if (peek() == ':') {
        consume(':');
        item = new YAMLItemKey(*item);
    }

    std::cout << "item -> " << (*item) << std::endl;

    item->attach();
    items_.push_back(item);

}

const YAMLItem& YAMLParser::nextItem() {
    loadItem();
    ASSERT(!items_.empty());

    YAMLItem* last = items_.front();
    items_.pop_front();
    return *last;
}

const YAMLItem& YAMLParser::peekItem() {
    loadItem();
    ASSERT(!items_.empty());
    return *items_.front();
}

Value YAMLParser::parseValue() {
    return nextItem().parse(*this);
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
