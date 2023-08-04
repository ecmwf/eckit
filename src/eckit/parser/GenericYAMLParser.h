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

#pragma once

#include <deque>

#include "eckit/parser/GenericObjectParser.h"

#include <algorithm>
#include <fstream>

#include "eckit/memory/Counted.h"
#include "eckit/types/Time.h"
#include "eckit/utils/Regex.h"
#include "eckit/utils/Translator.h"
#include "eckit/utils/VariantHelpers.h"
#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


namespace parser {
template <typename ObjectBuilder>
class YAMLItem;
};

using parser::ObjectValueTag;
using parser::ObjectValueType;

template <typename ObjectBuilder, typename ObjectParserBase = GenericObjectParser<ObjectBuilder>>
class GenericYAMLParser : public ObjectParserBase {
public:  // methods
    using Traits     = typename parser::ObjectBuilderTraits<ObjectBuilder>;
    using ValueType  = typename Traits::ValueType;
    using StringType = typename Traits::StringType;
    using NumberType = typename Traits::NumberType;

    GenericYAMLParser(std::istream& in);
    virtual ~GenericYAMLParser() override{};

    static ValueType decodeFile(const PathName& path);
    static ValueType decodeString(const std::string& str);

protected:
    // Need to be overloaded to allow parseJSON to support flow style keys in YAML
    std::string parseString(char) override;
    NumberType parseNumber() override;

private:
    std::optional<parser::YAMLItem<ObjectBuilder>> lastItem_;

    std::vector<char> stop_;
    std::vector<char> comma_;
    std::vector<char> colon_;

    std::map<std::string, ValueType> anchors_;

private:
    void loadItem();
    parser::YAMLItem<ObjectBuilder> nextItem();
    const parser::YAMLItem<ObjectBuilder>& peekItem();

    std::string nextWord();
    size_t consumeChars(char);
    ValueType consumeJSON(char);


    std::string nextToken(size_t& spaces, size_t& lines);

    bool endOfToken(char);

    ValueType parseValue() override;

    /**
     *
     * @param func Function with first argument overloaded of a scalar value (either `Traits::NullType`, `Traits::StringType`, `Traits::NumberType` or `Traits::BoolType`).
     *             An additional overload with `Traits::StringType` is possible with a second argument of type `std::true_type` indicating if it's a key.
     */
    template <typename Function>
    auto withParsedScalarNullOrKey(Function&& func) -> decltype(std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Null>{}),
                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::String>{}, std::declval<std::string&&>()),
                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Key>{}, std::declval<std::string&&>()),
                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{}, std::declval<long long>()),
                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{}, std::declval<double>()),
                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Bool>{}, std::declval<bool>()));

    std::string parserName() const override;

    const ValueType& anchor(std::string&& key, ValueType&& value);
    const ValueType& anchor(const std::string& key) const;

    friend class parser::YAMLItem<ObjectBuilder>;
};


//----------------------------------------------------------------------------------------------------------------------

namespace parser {

enum class YAMLItemType : unsigned long
{
    EndOfFile = 0,
    EndDocument,
    StartDocument,
    Key,  // has key type
    Entry,
    Anchor,     // has string type
    Reference,  // has string type
    Value,      // has value
    String,     // has string type - listed separatedly to be used before casting to StringType
};

template <YAMLItemType val>
struct YAMLItemTag { static constexpr YAMLItemType value = val; };

using YAMLItemEOF           = YAMLItemTag<YAMLItemType::EndOfFile>;
using YAMLItemEndOfFile     = YAMLItemTag<YAMLItemType::EndOfFile>;
using YAMLItemEndDocument   = YAMLItemTag<YAMLItemType::EndDocument>;
using YAMLItemStartDocument = YAMLItemTag<YAMLItemType::StartDocument>;
using YAMLItemKey           = YAMLItemTag<YAMLItemType::Key>;
using YAMLItemEntry         = YAMLItemTag<YAMLItemType::Entry>;
using YAMLItemAnchor        = YAMLItemTag<YAMLItemType::Anchor>;
using YAMLItemReference     = YAMLItemTag<YAMLItemType::Reference>;
using YAMLItemValue         = YAMLItemTag<YAMLItemType::Value>;
using YAMLItemString        = YAMLItemTag<YAMLItemType::String>;

template <typename ObjectBuilder>
class YAMLItem {
public:
    using Traits = ObjectBuilderTraits<ObjectBuilder>;

    long indent;
    YAMLItemType type;
    ObjectValueType valueType = ObjectValueType::Null;  // Non-null for Value

    struct None {};

    using KeyType       = typename Traits::KeyType;
    using StringType    = typename Traits::StringType;
    using StdStringType = std::string;
    using ValueType     = typename Traits::ValueType;

    union value_type {
        None none;
        KeyType key;
        StdStringType stringValue;
        StringType string;
        ValueType value;

        ~value_type(){};
    };

    value_type value{None{}};

    ~YAMLItem() {
        switch (type) {
            case YAMLItemType::Key:
                value.key.~KeyType();
                break;
            case YAMLItemType::Anchor:
            case YAMLItemType::Reference:
                value.string.~StringType();
                break;
            case YAMLItemType::String:
                value.stringValue.~StdStringType();
                break;
            case YAMLItemType::Value:
                value.value.~ValueType();
                break;
            case YAMLItemType::EndDocument:
            case YAMLItemType::StartDocument:
            case YAMLItemType::Entry:
            default:
                break;
        }
        value.none = None{};
    }

    YAMLItem(YAMLItem&& other) noexcept :
        indent{other.indent},
        type{other.type},
        value{None{}} {
        switch (type) {
            case YAMLItemType::Key:
                new (&value.key) KeyType{std::move(other.value.key)};
                break;
            case YAMLItemType::Anchor:
            case YAMLItemType::Reference:
                new (&value.string) StringType{std::move(other.value.string)};
                break;
            case YAMLItemType::String:
                new (&value.stringValue) StdStringType{std::move(other.value.stringValue)};
                break;
            case YAMLItemType::Value:
                new (&value.value) ValueType{std::move(other.value.value)};
                break;
            case YAMLItemType::EndDocument:
            case YAMLItemType::StartDocument:
            case YAMLItemType::Entry:
            default:
                break;
        }
    }

    YAMLItem(YAMLItemEOF t) :
        indent{-1}, type{t.value} {}
    YAMLItem(YAMLItemEndDocument t) :
        indent{-1}, type{t.value} {}
    YAMLItem(YAMLItemStartDocument t) :
        indent{-1}, type{t.value} {}
    YAMLItem(YAMLItemEntry t, long indent_) :
        indent{indent_}, type{t.value} {}
    YAMLItem(YAMLItemKey t, long indent_, KeyType&& k) :
        indent{indent_}, type{t.value} {
        new (&value.key) KeyType{std::move(k)};
    }
    YAMLItem(YAMLItemAnchor t, long indent_, StringType&& s) :
        indent{indent_}, type{t.value} {
        new (&value.string) StringType{std::move(s)};
    }
    YAMLItem(YAMLItemReference t, long indent_, StringType&& s) :
        indent{indent_}, type{t.value} {
        new (&value.string) StringType{std::move(s)};
    }
    YAMLItem(YAMLItemValue t, long indent_, ObjectValueType vt, ValueType&& v) :
        indent{indent_}, type{t.value}, valueType{vt} {
        new (&value.value) ValueType{std::move(v)};
    }
    YAMLItem(YAMLItemString t, long indent_, StdStringType&& v) :
        indent{indent_}, type{t.value}, valueType{ObjectValueType::String} {
        new (&value.string) StdStringType{std::move(v)};
    }

    friend std::ostream& operator<<(std::ostream& s, const YAMLItem& item) {
        switch (item.type) {
            case YAMLItemType::EndOfFile:
                s << "YAMLItemEndOfFile";
                break;
            case YAMLItemType::EndDocument:
                s << "YAMLItemEndDocument";
                break;
            case YAMLItemType::StartDocument:
                s << "YAMLItemStartDocument";
                break;
            case YAMLItemType::Entry:
                s << "YAMLItemEntry[indent=" << item.indent << "]";
                break;
            case YAMLItemType::Key:
                s << "YAMLItemKey[value=" << item.value.key << ", indent=" << item.indent << "]";
                break;
            case YAMLItemType::Anchor:
                s << "YAMLItemAnchor[value=" << item.value.string << ", indent=" << item.indent << "]";
                break;
            case YAMLItemType::Reference:
                s << "YAMLItemReference[value=" << item.value.string << ", indent=" << item.indent << "]";
                break;
            case YAMLItemType::Value:
                s << "YAMLItemValue[value=" << item.value.value << ", indent=" << item.indent << "]";
                break;
            case YAMLItemType::String:
                s << "YAMLItemString[value=" << item.value.stringValue << ", indent=" << item.indent << "]";
                break;
        }
        return s;
    }

    template <typename ObjectParserBase>
    ValueType parse(GenericYAMLParser<ObjectBuilder, ObjectParserBase>& parser) && {
        switch (type) {
            case YAMLItemType::EndOfFile:
            case YAMLItemType::EndDocument:
                return ObjectBuilder::from(ObjectValueTag<ObjectValueType::Null>{});
            case YAMLItemType::StartDocument:
                return std::move(*this).parseStartDocument(parser);
            case YAMLItemType::Entry:
                return std::move(*this).parseEntry(parser);
            case YAMLItemType::Key:
                return std::move(*this).parseKey(parser);
            case YAMLItemType::Anchor:
                return std::move(*this).parseAnchor(parser);
            case YAMLItemType::Reference:
                return std::move(*this).parseReference(parser);
            case YAMLItemType::Value:
                // String should be explicitly not passed in a value - to be accessed without revisiting it
                ASSERT(valueType != ObjectValueType::String);
                return std::move(value.value);
            case YAMLItemType::String:
                return ObjectBuilder::from(ObjectValueTag<ObjectValueType::String>{}, std::move(value.stringValue));
        }
    }

protected:
    template <typename ObjectParserBase>
    ValueType parseStartDocument(GenericYAMLParser<ObjectBuilder, ObjectParserBase>& parser) && {
        auto l = ObjectBuilder::from(ObjectValueTag<ObjectValueType::List>{});

        bool isFirst = true;
        for (;;) {
            auto v = parser.parseValue();

            for (bool peekLoop = true; peekLoop;) {
                const YAMLItem<ObjectBuilder>& next = parser.peekItem();
                switch (next.type) {
                    case YAMLItemType::EndOfFile:
                        if (isFirst) {
                            return std::move(v);
                        }
                        else {
                            ObjectBuilder::addToList(l, std::move(v));
                            return ObjectBuilder::toValue(std::move(l));
                        }
                        break;
                    case YAMLItemType::EndDocument:
                        // loop another round and check if theres more
                        parser.nextItem();
                        break;
                    default:
                        // There should be more... parse next value...
                        isFirst = false;
                        ObjectBuilder::addToList(l, std::move(v));
                        peekLoop = false;
                        break;
                }
            }
        }
        // Unreachable
        // return ObjectBuilder::toValue(std::move(l));
        throw SeriousBug("Did not reach EndDocument (...) or EndOfFile.");
    }

    template <typename ObjectParserBase>
    ValueType parseEntry(GenericYAMLParser<ObjectBuilder, ObjectParserBase>& parser) && {
        auto l = ObjectBuilder::from(ObjectValueTag<ObjectValueType::List>{});

        for (;;) {
            const YAMLItem& next = parser.peekItem();

            if (next.indent == indent) {
                // Special case
                ObjectBuilder::addToList(l, ObjectBuilder::from(ObjectValueTag<ObjectValueType::Null>{}));  // null
                YAMLItem advance = parser.nextItem();
                ASSERT(advance.type == YAMLItemType::Entry);
                continue;
            }

            if (next.indent < indent) {
                // Special case
                ObjectBuilder::addToList(l, ObjectBuilder::from(ObjectValueTag<ObjectValueType::Null>{}));  // null
                break;
            }

            if (next.indent > indent) {
                ObjectBuilder::addToList(l, parser.nextItem().parse(parser));  // null
            }

            const YAMLItem& peek = parser.peekItem();

            if (peek.indent < indent) {
                break;
            }

            if (peek.indent == indent) {
                YAMLItem advance = parser.nextItem();
                ASSERT(advance.type == YAMLItemType::Entry);
                continue;
            }

            std::ostringstream oss;
            oss << "YAMLItemEntry: Invalid sequence \"" << *this << "\" then " << next << " then " << peek << std::endl;
            throw eckit::SeriousBug(oss.str());
        }

        return ObjectBuilder::toValue(std::move(l));
    }

    template <typename ObjectParserBase>
    ValueType parseKey(GenericYAMLParser<ObjectBuilder, ObjectParserBase>& parser) && {
        auto m = ObjectBuilder::from(ObjectValueTag<ObjectValueType::Map>{});

        std::string keyValue = std::move(value.key);
        long keyIndent       = indent;

        auto stepNextKey = [&keyValue, &keyIndent, &parser]() {
            YAMLItem<ObjectBuilder> nextKey = parser.nextItem();
            ASSERT(nextKey.type == YAMLItemType::Key);
            keyValue  = std::move(nextKey.value.key);
            keyIndent = nextKey.indent;
        };

        for (;;) {
            // Test next value
            {
                const YAMLItem<ObjectBuilder>& next = parser.peekItem();

                if (next.indent == keyIndent) {
                    // Special case
                    ObjectBuilder::addToMapIfNotContained(m, std::move(keyValue), ObjectBuilder::from(ObjectValueTag<ObjectValueType::Null>{}));  // null

                    stepNextKey();
                    continue;
                }

                if (next.indent < keyIndent) {
                    // Special case
                    ObjectBuilder::addToMapIfNotContained(m, std::move(keyValue), ObjectBuilder::from(ObjectValueTag<ObjectValueType::Null>{}));  // null

                    break;
                }

                ASSERT(next.indent > keyIndent);
            }

            // Acquire next value
            YAMLItem<ObjectBuilder> next = parser.nextItem();
            std::optional<std::string> stringVal;
            std::optional<ValueType> otherVal;
            // If string is contained, safe explicitly to potential reuse if the next elements are already strings...
            if (next.type == YAMLItemType::String) {
                stringVal = std::move(next.value.stringValue);
            }
            else {
                // Something else is contained, need to parse explicitly
                otherVal = std::move(next).parse(parser);
            }
            static std::string import("<<");


            bool doImport     = keyValue == import;
            auto addLastValue = [&m, &keyValue, &doImport, &stringVal, &otherVal]() {
                if (doImport) {
                    if (otherVal) {
                        ObjectBuilder::visit(
                            *otherVal,
                            Overloaded{
                                [&m](ObjectValueTag<ObjectValueType::Map>, const auto& map) -> void {
                                    ObjectBuilder::forEachMap(map, [&m](const auto& k2, const auto& v2) -> void {
                                        ObjectBuilder::addToMapIfNotContained(m, k2, v2);
                                    });
                                },
                                [&m](ObjectValueTag<ObjectValueType::List>, const auto& list) -> void {
                                    ObjectBuilder::forEachList(list, [&m](const auto& listVal) {
                                        ObjectBuilder::visit(listVal,
                                                             Overloaded{
                                                                 [&m](ObjectValueTag<ObjectValueType::Map>, const auto& map) -> void {
                                                                     ObjectBuilder::forEachMap(map, [&m](const auto& k2, const auto& v2) -> void {
                                                                         ObjectBuilder::addToMapIfNotContained(m, k2, v2);
                                                                     });
                                                                 },
                                                                 [](auto, const auto&) {}});
                                    });
                                },
                                [](auto, const auto&) -> void {
                                    // Not forbidden to reach but useless. Merge Key Language-Independent Type is only working for maps or list of maps.
                                }});
                    }
                }
                else {
                    if (stringVal) {
                        ObjectBuilder::addToMapIfNotContained(m, std::move(keyValue), std::move(*stringVal));
                    }
                    else if (otherVal) {
                        ObjectBuilder::addToMapIfNotContained(m, std::move(keyValue), std::move(*otherVal));
                    }
                }
            };

            // Peek new item to check if the value is an extended multiline string, get next key or stop
            const YAMLItem& peek
                = parser.peekItem();

            if (peek.indent < keyIndent) {
                addLastValue();
                break;
            }

            if (peek.indent == keyIndent) {
                addLastValue();
                stepNextKey();
                continue;
            }

            // Special case of multi line string
            if (!doImport && stringVal && peek.indent > keyIndent && (peek.type == YAMLItemType::String)) {
                std::ostringstream oss;

                oss << std::move(*stringVal);
                for (;;) {
                    const YAMLItem& peek = parser.peekItem();

                    if (!(peek.indent > keyIndent && (peek.type == YAMLItemType::String))) {
                        break;
                    }

                    oss << ' ' << parser.nextItem().value.stringValue;
                }

                ObjectBuilder::addToMapIfNotContained(m, std::move(keyValue), ObjectBuilder::from(ObjectValueTag<ObjectValueType::String>{}, oss.str()));
                continue;
            }

            std::ostringstream oss;
            oss << "YAMLItemKey: Invalid sequence \"" << keyValue << "\" then " << next << " then " << peek << std::endl;
            throw eckit::SeriousBug(oss.str());
        }

        return ObjectBuilder::toValue(std::move(m));
    }

    template <typename ObjectParserBase>
    const ValueType& parseAnchor(GenericYAMLParser<ObjectBuilder, ObjectParserBase>& parser) && {
        return parser.anchor(std::move(value.string), parser.nextItem().parse(parser));
    }

    template <typename ObjectParserBase>
    const ValueType& parseReference(GenericYAMLParser<ObjectBuilder, ObjectParserBase>& parser) && {
        return parser.anchor(value.string);
    }
};


//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder, typename Function>
static auto withInterpretedScalarOrNull(std::string&& s, Function&& func) -> decltype(std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Null>{}),
                                                                                      std::forward<Function>(func)(ObjectValueTag<ObjectValueType::String>{},
                                                                                                                   std::declval<std::string&&>()),
                                                                                      std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{},
                                                                                                                   std::declval<long long>()),
                                                                                      std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{},
                                                                                                                   std::declval<double>()),
                                                                                      std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Bool>{},
                                                                                                                   std::declval<bool>())) {
    static Regex integer8("^0o[0-7_]+$", false);
    static Regex integer10("^[-+]?[0-9_]+$", false);
    static Regex integer16("0x[0-9a-fA-F_]+$", false);
    static Regex float10(
        "^[-+]?(\\.[0-9_]+|[0-9_]+(\\.[0-9_]*)?)([eE][-+]?[0-9]+)?$",
        false);
    static Regex floatspecial(
        "^(\\.(nan|NaN|NAN)|[-+]?\\.(inf|Inf|INF))$",
        false);
    // static Regex time("[0-9]+:[0-9]+:[0-9]+$", false);

    auto fwdNumber = [&func](auto v) {
        return std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{}, v);
    };
    auto fwdBool = [&func](auto v) {
        return std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Bool>{}, v);
    };
    auto fwdNull = [&func]() {
        return std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Null>{});
    };
    auto fwdString = [&func](auto&& v) {
        return std::forward<Function>(func)(ObjectValueTag<ObjectValueType::String>{}, std::move(v));
    };

    /*
    if (time.match(s)) {
        return Value(Time(s));
    }
    */

    // std::cout << "TO VALUE " << s << std::endl;

    if (s.length()) {
        // This is because checking regex is very slow
        switch (s[0]) {
            case '-':
            case '+':
            case '0':

                if (integer8.match(s)) {
                    s.erase(remove(s.begin(), s.end(), '_'), s.end());
                    return fwdNumber(strtol(s.substr(2).c_str(), 0, 8));
                }

                if (integer16.match(s)) {
                    s.erase(remove(s.begin(), s.end(), '_'), s.end());
                    return fwdNumber(strtol(s.substr(2).c_str(), 0, 16));
                }

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':

                if (integer10.match(s)) {
                    s.erase(remove(s.begin(), s.end(), '_'), s.end());
                    return fwdNumber(translate<long long>(std::move(s)));
                }

            case '.':

                if (float10.match(s)) {
                    s.erase(remove(s.begin(), s.end(), '_'), s.end());
                    return fwdNumber(translate<double>(std::move(s)));
                }

                if (floatspecial.match(s)) {
                    s.erase(remove(s.begin(), s.end(), '.'), s.end());
                    return fwdNumber(translate<double>(std::move(s)));
                }

                break;

            case 'o':
                if (s == "on") {
                    return fwdBool(true);
                }

                if (s == "off") {
                    return fwdBool(false);
                }

                break;

            case 'n':
            case 'N':
                if (s == "null" || s == "Null" || s == "NULL") {
                    return fwdNull();
                }

                if (s == "no") {
                    return fwdBool(false);
                }

                break;

            case 'f':
            case 'F':
                if (s == "false" || s == "False" || s == "FALSE") {
                    return fwdBool(false);
                }
                break;

            case 't':
            case 'T':
                if (s == "true" || s == "True" || s == "TRUE") {
                    return fwdBool(true);
                }
                break;

            case 'y':
                if (s == "yes") {
                    return fwdBool(true);
                }
                break;

            case '"':
                ASSERT(s[0] != '"');
                break;

            case '\'':
                ASSERT(s[0] != '\'');
                break;
            case '~':
                if (s == "~") {
                    return fwdNull();
                }
                break;
        }
    }

    return fwdString(std::move(s));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace parser

//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder, typename ObjectParserBase>
template <typename Function>
auto GenericYAMLParser<ObjectBuilder, ObjectParserBase>::withParsedScalarNullOrKey(Function&& func) -> decltype(std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Null>{}),
                                                                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::String>{}, std::declval<std::string&&>()),
                                                                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Key>{}, std::declval<std::string&&>()),
                                                                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{}, std::declval<long long>()),
                                                                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Number>{}, std::declval<double>()),
                                                                                                                std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Bool>{}, std::declval<bool>())) {
    auto fwdString = [&func](auto&& v) {
        return std::forward<Function>(func)(ObjectValueTag<ObjectValueType::String>{}, ObjectBuilder::from(ObjectValueTag<ObjectValueType::String>{}, std::move(v)));
    };
    auto fwdKey = [&func](auto&& v) {
        return std::forward<Function>(func)(ObjectValueTag<ObjectValueType::Key>{}, ObjectBuilder::from(ObjectValueTag<ObjectValueType::Key>{}, std::move(v)));
    };

    bool multi
        = false;
    bool folded = false;
    bool string = false;
    char c      = this->peek();

    if (c == '"' || c == '\'') {
        auto result = ObjectParserBase::parseString(c);
        if (this->peek(true) == ':') {
            return fwdKey(std::move(result));
        }
        else {
            return fwdString(std::move(result));
        }
    }

    if (c == '|') {
        this->consume('|');
        multi  = true;
        string = true;
    }

    if (c == '>') {
        this->consume('>');
        folded = true;
        string = true;
    }


    c             = this->peek();
    size_t indent = this->pos_;
    size_t line   = this->line_;

    std::string result;

    bool was_indented = false;

    while (this->pos_ >= indent) {

        size_t start    = this->pos_;
        bool add_cr     = (folded && (this->pos_ != indent)) || multi || was_indented;
        bool add_indent = (folded && (this->pos_ != indent)) || multi;
        bool isKey      = false;
        std::string s;
        size_t last = 0;
        size_t i    = 0;

        bool colon = (c == ':');

        while (!endOfToken(c)) {
            char p = this->next(true);
            if (!::isspace(p)) {
                last = i;
            }
            c = this->peek(true);
            i++;

            // std::cout << "++++ " << s << " " << colon << " " << (endOfToken(c) || c == ' ') << std::endl;

            if (colon && (endOfToken(c) || c == ' ')) {
                while (s.length() >= 1 && isspace(s[s.length() - 1])) {
                    s = s.substr(0, s.length() - 1);
                    last--;
                }

                // std::cout << s << std::endl;

                isKey = true;
                break;
            }
            s += p;

            colon = (c == ':');
        }

        if (result.size()) {
            if (add_cr) {
                for (size_t i = line; i < this->line_; i++) {
                    result += '\n';
                }
            }
            else {
                result += ' ';
            }
        }

        if (add_indent) {
            for (size_t i = indent; i < start; i++) {
                result += ' ';
            }
        }


        result += s.substr(0, last + 1);
        line = this->line_;

        if (isKey) {
            return fwdKey(std::move(result));
        }


        c = this->peek();

        if (this->endOfToken(c)) {
            break;
        }

        was_indented = add_indent;
    }

    if (string) {
        for (size_t i = line; i < this->line_; i++) {
            result += '\n';
        }
        return fwdString(std::move(result));
    }

    return parser::withInterpretedScalarOrNull<ObjectBuilder>(std::move(result), std::forward<Function>(func));
}

//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder, typename ObjectParserBase>
GenericYAMLParser<ObjectBuilder, ObjectParserBase>::GenericYAMLParser(std::istream& in) :
    ObjectParserBase(in, true, true) {
    stop_.push_back(0);
    comma_.push_back(0);
    colon_.push_back(0);
}

template <typename ObjectBuilder, typename ObjectParserBase>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType GenericYAMLParser<ObjectBuilder, ObjectParserBase>::decodeFile(const PathName& path) {
    std::ifstream in(std::string(path).c_str());
    if (!in) {
        throw eckit::CantOpenFile(path);
    }
    return GenericYAMLParser<ObjectBuilder, ObjectParserBase>{in}.parse();
}

template <typename ObjectBuilder, typename ObjectParserBase>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType GenericYAMLParser<ObjectBuilder, ObjectParserBase>::decodeString(const std::string& str) {
    std::istringstream in(str);
    return GenericYAMLParser<ObjectBuilder, ObjectParserBase>{in}.parse();
}


template <typename ObjectBuilder, typename ObjectParserBase>
std::string GenericYAMLParser<ObjectBuilder, ObjectParserBase>::nextWord() {
    std::string word;
    char c = this->peek(true);

    while (!(::isspace(c) || c == 0 || c == '\n')) {
        word += this->next();
        c = this->peek(true);
    }

    return word;
}

template <typename ObjectBuilder, typename ObjectParserBase>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType GenericYAMLParser<ObjectBuilder, ObjectParserBase>::consumeJSON(char ket) {
    stop_.push_back(ket);
    comma_.push_back(',');
    colon_.push_back(':');
    ValueType v = this->parseJSON();
    stop_.pop_back();
    comma_.pop_back();
    colon_.pop_back();
    return v;
}


template <typename ObjectBuilder, typename ObjectParserBase>
size_t GenericYAMLParser<ObjectBuilder, ObjectParserBase>::consumeChars(char which) {
    char c     = this->peek(true);
    size_t cnt = 0;

    while (c == which) {
        this->consume(which);
        c = this->peek(true);
        cnt++;
    }

    if (c == ' ' || c == '\n') {
        return cnt;
    }

    while (cnt-- > 0) {
        this->putback(which);
    }
    return 0;
}

template <typename ObjectBuilder, typename ObjectParserBase>
bool GenericYAMLParser<ObjectBuilder, ObjectParserBase>::endOfToken(char c) {
    return (c == '\n' || c == 0 || c == stop_.back() || c == comma_.back() || c == colon_.back());
}


template <typename ObjectBuilder, typename ObjectParserBase>
void GenericYAMLParser<ObjectBuilder, ObjectParserBase>::loadItem() {
    if (lastItem_) {
        return;
    }

    char c = this->peek();

    size_t indent = this->pos_;

    std::string key;
    size_t cnt = 0;

    auto yamlItemFromScalar = [this, &indent]() {
        withParsedScalarNullOrKey(Overloaded{
            [this, &indent](ObjectValueTag<ObjectValueType::String>, std::string&& v) {
                this->lastItem_.emplace(parser::YAMLItemString{}, indent, std::move(v));
            },
            [this, &indent](ObjectValueTag<ObjectValueType::Key> t, auto&& v) {
                this->next(true);
                this->lastItem_.emplace(parser::YAMLItemKey{}, indent, ObjectBuilder::from(t, std::move(v)));
            },
            [this, &indent](ObjectValueTag<ObjectValueType::Null> typeTag) {
                this->lastItem_.emplace(parser::YAMLItemValue{}, indent, typeTag.value, ObjectBuilder::toValue(ObjectBuilder::from(typeTag)));
            },
            [this, &indent](auto typeTag, auto&& v) {
                this->lastItem_.emplace(parser::YAMLItemValue{}, indent, typeTag.value, ObjectBuilder::toValue(ObjectBuilder::from(typeTag, std::forward<decltype(v)>(v))));
            }});
    };


    switch (c) {

        case 0:
            lastItem_.emplace(parser::YAMLItemEOF{});
            break;

        case '{':
            lastItem_.emplace(parser::YAMLItemValue{}, indent, ObjectValueType::Map, consumeJSON('}'));
            break;

        case '[':
            lastItem_.emplace(parser::YAMLItemValue{}, indent, ObjectValueType::List, consumeJSON(']'));
            break;

        case '"':
        case '\'':
            yamlItemFromScalar();
            break;

        case '-':

            cnt = this->consumeChars('-');

            switch (cnt) {
                case 1:
                    lastItem_.emplace(parser::YAMLItemEntry{}, indent + 1);
                    break;

                case 3:
                    lastItem_.emplace(parser::YAMLItemStartDocument{});
                    break;

                default:
                    while (cnt--) {
                        this->putback('-');
                    }
                    yamlItemFromScalar();
                    break;
            }

            break;

        case '.':

            cnt = this->consumeChars('.');

            switch (cnt) {

                case 3:
                    lastItem_.emplace(parser::YAMLItemEndDocument{});
                    break;

                default:
                    while (cnt--) {
                        this->putback('.');
                    }
                    yamlItemFromScalar();
                    break;
            }

            break;

        case '&':
            this->consume('&');
            lastItem_.emplace(parser::YAMLItemAnchor{}, indent, nextWord());
            break;

        case '*':
            this->consume('*');
            lastItem_.emplace(parser::YAMLItemReference{}, indent, nextWord());
            break;

        default:
            yamlItemFromScalar();
            break;
    }
}


template <typename ObjectBuilder, typename ObjectParserBase>
const typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType& GenericYAMLParser<ObjectBuilder, ObjectParserBase>::anchor(std::string&& key, typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType&& value) {
    // YAML 1.2 - refer to most RECEST anchor
    return (anchors_.insert_or_assign(std::move(key), std::move(value)).first)->second;
}

template <typename ObjectBuilder, typename ObjectParserBase>
const typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType& GenericYAMLParser<ObjectBuilder, ObjectParserBase>::anchor(const std::string& key) const {
    auto j = anchors_.find(key);
    if (j == anchors_.end()) {
        std::ostringstream oss;
        oss << "YAMLParser: cannot find anchor [" << key << "]";
        throw eckit::UserError(oss.str());
    }
    return j->second;
}

template <typename ObjectBuilder, typename ObjectParserBase>
parser::YAMLItem<ObjectBuilder> GenericYAMLParser<ObjectBuilder, ObjectParserBase>::nextItem() {
    loadItem();
    ASSERT(lastItem_);

    parser::YAMLItem<ObjectBuilder> last{std::move(*lastItem_)};
    lastItem_ = std::nullopt;
    return last;
}

template <typename ObjectBuilder, typename ObjectParserBase>
const parser::YAMLItem<ObjectBuilder>& GenericYAMLParser<ObjectBuilder, ObjectParserBase>::peekItem() {
    loadItem();
    ASSERT(lastItem_);
    return *lastItem_;
}

template <typename ObjectBuilder, typename ObjectParserBase>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType GenericYAMLParser<ObjectBuilder, ObjectParserBase>::parseValue() {
    return nextItem().parse(*this);
}

template <typename ObjectBuilder, typename ObjectParserBase>
std::string GenericYAMLParser<ObjectBuilder, ObjectParserBase>::parserName() const {
    return "YAMLParser";
}

template <typename ObjectBuilder, typename ObjectParserBase>
std::string GenericYAMLParser<ObjectBuilder, ObjectParserBase>::parseString(char) {
    // This function will be called when parsing JSON objects in YAML. YAML allows unquoted key and even NULL.
    return withParsedScalarNullOrKey(Overloaded{
        [](ObjectValueTag<ObjectValueType::String>, std::string&& v) -> std::string {
            return std::move(v);
        },
        [](ObjectValueTag<ObjectValueType::Key>, std::string&& v) -> std::string {
            return std::move(v);
        },
        [](ObjectValueTag<ObjectValueType::Null>) -> std::string {
            return "";
        },
        [](ObjectValueTag<ObjectValueType::Bool>, bool b) -> std::string {
            return b ? "true" : "false";
        },
        [](ObjectValueTag<ObjectValueType::Number>, auto v) -> std::string {
            return translate<std::string>(v);
        }});
}

template <typename ObjectBuilder, typename ObjectParserBase>
typename parser::ObjectBuilderTraits<ObjectBuilder>::NumberType GenericYAMLParser<ObjectBuilder, ObjectParserBase>::parseNumber() {
    return withParsedScalarNullOrKey(Overloaded{
        [](ObjectValueTag<ObjectValueType::String>, auto&&) {
            return ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, 0);
        },
        [](ObjectValueTag<ObjectValueType::Key>, auto&&) {
            return ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, 0);
        },
        [](ObjectValueTag<ObjectValueType::Null>) {
            return ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, 0);
        },
        [](ObjectValueTag<ObjectValueType::Bool>, bool b) {
            return ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, b ? 1 : 0);
        },
        [](ObjectValueTag<ObjectValueType::Number>, auto v) {
            return ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, v);
        }});
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
