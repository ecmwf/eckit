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

#include "eckit/parser/StreamParser.h"
#include "eckit/utils/Translator.h"
#include "eckit/value/Value.h"

#include "eckit/eckit_config.h"

#include <string>
#include <sstream>
#include <type_traits>

#if eckit_HAVE_UNICODE
#include <codecvt>
#endif /* eckit_HAVE_UNICODE */

#include <locale>


namespace eckit {

namespace parser {

enum class ObjectValueType : unsigned long
{
    Null = 0,
    Bool,
    String,
    Number,
    List,
    Map,
    Key
};

template <ObjectValueType val>
struct ObjectValueTag { static constexpr ObjectValueType value = val; };


template <typename ObjectBuilder>
struct ObjectBuilderTraits {
    using BoolType   = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Bool>{}, std::declval<bool>()));
    using NullType   = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Null>{}));
    using StringType = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::String>{}, std::declval<std::string&&>()));
    
    // static_assert(std::is_same<std::string, decltype((std::string) std::declval<StringType>())>::value, "StringType should be convertible to std::string.");

    // using KeyType = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Key>{}, std::declval<StringType&&>()), ObjectBuilder::from(ObjectValueTag<ObjectValueType::Key>{}, std::declval<std::string&&>()));
    using KeyType = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Key>{}, std::declval<std::string&&>()));
    
    // static_assert(std::is_same<std::string, decltype((std::string) std::declval<KeyType>())>::value, "KeyType should be convertible to std::string.");

    using IntType    = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, std::declval<int>()));
    using DoubleType = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Number>{}, std::declval<double>()));

    static_assert(std::is_same<IntType, DoubleType>::value, "Integer and double types are supposed to resolve in the same NumberType");
    using NumberType = IntType;

    using MapType  = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::Map>{}));
    using ListType = decltype(ObjectBuilder::from(ObjectValueTag<ObjectValueType::List>{}));

    using ValueType = decltype(ObjectBuilder::toValue(std::declval<NullType&&>()));
    static_assert(std::is_same<ValueType, decltype(ObjectBuilder::toValue(std::declval<BoolType&&>()))>::value, "ObjectBuilder::toValue(BoolType) should return the same type like calls to toValue(...) with other types.");
    static_assert(std::is_same<ValueType, decltype(ObjectBuilder::toValue(std::declval<StringType&&>()))>::value, "ObjectBuilder::toValue(StringType) should return the same type like calls to toValue(...) with other types.");
    static_assert(std::is_same<ValueType, decltype(ObjectBuilder::toValue(std::declval<NumberType&&>()))>::value, "ObjectBuilder::toValue(NumberType) should return the same type like calls to toValue(...) with other types.");
    static_assert(std::is_same<ValueType, decltype(ObjectBuilder::toValue(std::declval<ListType&&>()))>::value, "ObjectBuilder::toValue(ListType) should return the same type like calls to toValue(...) with other types.");
    static_assert(std::is_same<ValueType, decltype(ObjectBuilder::toValue(std::declval<MapType&&>()))>::value, "ObjectBuilder::toValue(MapType) should return the same type like calls to toValue(...) with other types.");

    // Test if adding values to map is possible
    // To support legacy yaml - adding existing keys should be ignored
    static_assert(std::is_same<void, decltype(ObjectBuilder::addToMapIfNotContained(std::declval<MapType&>(), std::declval<KeyType&&>(), std::declval<ValueType&&>()))>::value, "ObjectBuilder should allow add key-values to a map type");
    static_assert(std::is_same<void, decltype(ObjectBuilder::addToMapIfNotContained(std::declval<MapType&>(), std::declval<const KeyType&>(), std::declval<const ValueType&>()))>::value, "ObjectBuilder should allow add key-values to a map type");

    static constexpr auto testForEachMapFuncCRef = [](const KeyType&, const ValueType&) {};
    static constexpr auto testForEachMapFuncRVal = [](KeyType&&, ValueType&&) {};
    static_assert(std::is_same<void, decltype(ObjectBuilder::forEachMap(std::declval<const MapType&>(), testForEachMapFuncCRef))>::value, "ObjectBuilder should allow to iterate a map");
    static_assert(std::is_same<void, decltype(ObjectBuilder::forEachMap(std::declval<MapType&&>(), testForEachMapFuncRVal))>::value, "ObjectBuilder should allow to iterate a map");

    // Test if adding values to list is possible
    static_assert(std::is_same<void, decltype(ObjectBuilder::addToList(std::declval<ListType&>(), std::declval<ValueType&&>()))>::value, "ObjectBuilder should allow add values to a list type");
    static_assert(std::is_same<void, decltype(ObjectBuilder::addToList(std::declval<ListType&>(), std::declval<const ValueType&>()))>::value, "ObjectBuilder should allow add values to a list type");


    static constexpr auto testForEachListFuncCRef = [](const ValueType&) {};
    static constexpr auto testForEachListFuncRVal = [](ValueType&&) {};
    static_assert(std::is_same<void, decltype(ObjectBuilder::forEachList(std::declval<const ListType&>(), testForEachListFuncCRef))>::value, "ObjectBuilder should allow to iterate a list");
    static_assert(std::is_same<void, decltype(ObjectBuilder::forEachList(std::declval<ListType&&>(), testForEachListFuncRVal))>::value, "ObjectBuilder should allow to iterate a list");


    static constexpr auto testVisitFuncCRef = [](auto tag, const auto& val) {
        return decltype(tag)::value;
    };
    static constexpr auto testVisitFuncRVal = [](auto tag, auto&& val) {
        return decltype(tag)::value;
    };
    static_assert(std::is_same<ObjectValueType, decltype(ObjectBuilder::visit(std::declval<const ValueType&>(), std::move(testVisitFuncCRef)))>::value,
                  "Allow visiting a value type with a function expecting a tag and its value");
    static_assert(std::is_same<ObjectValueType, decltype(ObjectBuilder::visit(std::declval<ValueType&&>(), std::move(testVisitFuncRVal)))>::value,
                  "Allow visiting a value type with a function expecting a tag and its value");
};
}  // namespace parser

//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
class GenericObjectParser : public StreamParser {
public:  // methods
    using Traits = parser::ObjectBuilderTraits<ObjectBuilder>;
    virtual ~GenericObjectParser() override{};

    virtual typename Traits::ValueType parse();

protected:
    GenericObjectParser(std::istream& in, bool comments, bool yaml);

protected:  // methods
    virtual typename Traits::BoolType parseTrue();
    virtual typename Traits::BoolType parseFalse();
    virtual typename Traits::NullType parseNull();
    virtual typename Traits::ValueType parseValue() = 0;
    virtual typename Traits::MapType parseObject();
    virtual typename Traits::ListType parseArray();
    // Using std::string because it is used internally and needs to be accessed through this interface to avoid 
    // casting back/and forth from a potentially different StringType
    virtual std::string parseString(char quote = '"');
    virtual typename Traits::NumberType parseNumber();

    virtual typename Traits::ValueType parseJSON();

    virtual void parseKeyValue(typename Traits::MapType&);

    virtual std::string parserName() const = 0;

private:
    std::string unicode();
    bool yaml_;
};


//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
GenericObjectParser<ObjectBuilder>::GenericObjectParser(std::istream& in, bool comments, bool yaml) :
    StreamParser(in, comments), yaml_(yaml) {}

//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::BoolType GenericObjectParser<ObjectBuilder>::parseTrue() {
    consume("true");
    return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Bool>{}, true);
}

template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::BoolType GenericObjectParser<ObjectBuilder>::parseFalse() {
    consume("false");
    return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Bool>{}, false);
}


//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::NullType GenericObjectParser<ObjectBuilder>::parseNull() {
    consume("null");
    return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Null>{});
}


//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::NumberType GenericObjectParser<ObjectBuilder>::parseNumber() {
    bool real = false;

    std::string s;
    char c = next();
    if (c == '-') {
        s += c;
        c = next();
    }

    switch (c) {
        case '0':
            s += c;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            s += c;
            while (isdigit(peek())) {
                s += next();
            }
            break;
        default:
            throw StreamParser::Error(std::string("ObjectParser::parseNumber invalid char '") + c + "'");
    }

    if (peek() == '.') {
        real = true;
        s += next();
        c = next();
        if (!isdigit(c)) {
            throw StreamParser::Error(std::string("ObjectParser::parseNumber invalid char '")
                                      + c + "'");
        }
        s += c;
        while (isdigit(peek())) {
            s += next();
        }
    }


    c = peek();
    if (c == 'e' || c == 'E') {
        real = true;
        s += next();

        c = next();
        if (c == '-' || c == '+') {
            s += c;
            c = next();
        }

        if (!isdigit(c)) {
            throw StreamParser::Error(std::string("ObjectParser::parseNumber invalid char '") + c
                                      + "'");
        }
        s += c;
        while (isdigit(peek())) {
            s += next();
        }
    }

    if (real) {
        return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Number>{}, translate<double>(std::move(s)));
    }
    return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Number>{}, translate<long long>(std::move(s)));
}


//----------------------------------------------------------------------------------------------------------------------

#if eckit_HAVE_UNICODE
static inline std::string utf8(uint32_t code) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(char32_t(code));
}


template <typename ObjectBuilder>
std::string GenericObjectParser<ObjectBuilder>::unicode() {
    std::string tmp;

    while (true) {
        char c = peek();

        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            consume(c);
            tmp += c;
        }
        else {
            break;
        }
    }


    std::istringstream iss(tmp);
    uint32_t code;
    iss >> std::hex >> code;

    // std::cout << " [" << code << ", " << utf8(code) << "]" << std::endl;

    return utf8(code);
}
#endif /* eckit_HAVE_UNICODE */


template <typename ObjectBuilder>
std::string GenericObjectParser<ObjectBuilder>::parseString(char quote) {
    bool save = comments_;
    comments_ = false;

    consume(quote);
    std::string s;
    for (;;) {
        char c = next(true);

        if (c == '\\') {
            if (yaml_ && quote == '\'') {
                // Escapes do not work in YAML single quotes
                s += '\\';
                s += next(true);
            }
            else {
                c = next(true);
                switch (c) {
                    case '\\':
                        s += '\\';
                        break;

                    case '/':
                        s += '/';
                        break;

                    case 'b':
                        s += '\b';
                        break;

                    case 'f':
                        s += '\f';
                        break;

                    case 'n':
                        s += '\n';
                        break;

                    case 'r':
                        s += '\r';
                        break;

                    case 't':
                        s += '\t';
                        break;

#if eckit_HAVE_UNICODE
                    case 'u':
                        s += unicode();
                        break;
#endif /* eckit_HAVE_UNICODE */

                    default:
                        if (c == quote) {
                            s += c;
                        }
                        else {
                            comments_ = save;
                            throw StreamParser::Error(std::string("ObjectParser::parseString invalid escaped char '") + c + "'");
                        }
                        break;
                }
            }
        }
        else {
            if (c == quote) {
                if (yaml_ && quote == '\'') {
                    if (peek() == '\'') {
                        consume('\'');
                        s += c;
                        continue;
                    }
                }

                comments_ = save;
                return s;
            }
            s += c;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
void GenericObjectParser<ObjectBuilder>::parseKeyValue(typename parser::ObjectBuilderTraits<ObjectBuilder>::MapType& m) {
    auto k = ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Key>{}, parseString());
    consume(':');
    auto v = parseValue();

    ObjectBuilder::addToMapIfNotContained(m, std::move(k), std::move(v));
}

template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::MapType GenericObjectParser<ObjectBuilder>::parseObject() {
    consume("{");
    char c = peek();
    if (c == '}') {
        consume(c);
        return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Map>{});
    }

    auto m = ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::Map>{});

    for (;;) {
        parseKeyValue(m);

        char c = peek();
        if (c == '}') {
            consume(c);
            return m;
        }

        consume(',');
    }
}

//----------------------------------------------------------------------------------------------------------------------


template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ListType GenericObjectParser<ObjectBuilder>::parseArray() {
    consume("[");
    char c = peek();
    if (c == ']') {
        consume(c);
        return ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::List>{});
    }

    auto l = ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::List>{});

    for (;;) {
        ObjectBuilder::addToList(l, parseValue());

        char c = peek();
        if (c == ']') {
            consume(c);
            // cout << "ObjectParser::parseArray <== " << std::endl;;
            return l;
        }

        consume(',');
    }
}

//----------------------------------------------------------------------------------------------------------------------


template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType GenericObjectParser<ObjectBuilder>::parseJSON() {
    char c = peek();
    switch (c) {
        case 't':
            return ObjectBuilder::toValue(parseTrue());
        case 'f':
            return ObjectBuilder::toValue(parseFalse());
        case 'n':
            return ObjectBuilder::toValue(parseNull());
        case '{':
            return ObjectBuilder::toValue(parseObject());
        case '[':
            return ObjectBuilder::toValue(parseArray());
        case '\"':
            return ObjectBuilder::toValue(ObjectBuilder::from(parser::ObjectValueTag<parser::ObjectValueType::String>{}, parseString()));

        case '-':
            return ObjectBuilder::toValue(parseNumber());
        case '0':
            return ObjectBuilder::toValue(parseNumber());
        case '1':
            return ObjectBuilder::toValue(parseNumber());
        case '2':
            return ObjectBuilder::toValue(parseNumber());
        case '3':
            return ObjectBuilder::toValue(parseNumber());
        case '4':
            return ObjectBuilder::toValue(parseNumber());
        case '5':
            return ObjectBuilder::toValue(parseNumber());
        case '6':
            return ObjectBuilder::toValue(parseNumber());
        case '7':
            return ObjectBuilder::toValue(parseNumber());
        case '8':
            return ObjectBuilder::toValue(parseNumber());
        case '9':
            return ObjectBuilder::toValue(parseNumber());

        default: {
            std::ostringstream oss;
            oss << parserName() << " ObjectParser::parseValue unexpected char ";
            if (isprint(c) && !isspace(c)) {
                oss << "'" << c << "'";
            }
            else {
                oss << int(c);
            }
            throw StreamParser::Error(oss.str());
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------

template <typename ObjectBuilder>
typename parser::ObjectBuilderTraits<ObjectBuilder>::ValueType GenericObjectParser<ObjectBuilder>::parse() {
    auto v = parseValue();
    char c = peek();
    if (c != 0) {
        std::ostringstream oss;
        oss << parserName() << " ObjectParser::parseValue extra char ";
        if (isprint(c) && !isspace(c)) {
            oss << "'" << c << "'";
        }
        else {
            oss << int(c);
        }
        throw StreamParser::Error(oss.str());
    }
    return v;
}

//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit
