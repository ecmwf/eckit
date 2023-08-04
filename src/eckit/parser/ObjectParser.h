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

#include "eckit/parser/GenericObjectParser.h"
#include "eckit/types/Types.h"
#include "eckit/value/Value.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace parser {

struct ValueBuilder {
    static Value from(ObjectValueTag<ObjectValueType::Null>) { return Value(); };

    static bool from(ObjectValueTag<ObjectValueType::Bool>, bool b) { return Value(b); };

    static std::string from(ObjectValueTag<ObjectValueType::String>, const std::string& s) { return s; };
    static std::string from(ObjectValueTag<ObjectValueType::String>, std::string&& s) { return std::move(s); };
    static std::string from(ObjectValueTag<ObjectValueType::Key>, const std::string& s) { return s; };
    static std::string from(ObjectValueTag<ObjectValueType::Key>, std::string&& s) { return std::move(s); };

    template <typename IntType, std::enable_if_t<std::is_integral<IntType>::value, bool> = true>
    static Value from(ObjectValueTag<ObjectValueType::Number>, IntType n) { return n; };

    template <typename FloatType, std::enable_if_t<std::is_floating_point<FloatType>::value, bool> = true>
    static Value from(ObjectValueTag<ObjectValueType::Number>, FloatType f) { return f; };


    struct MapOrderedKeysPair {
        ValueMap map;
        ValueList orderedKeys;
    };

    static MapOrderedKeysPair from(ObjectValueTag<ObjectValueType::Map>) {
        return MapOrderedKeysPair{};
    };

    static void addToMapIfNotContained(MapOrderedKeysPair& m, std::string&& k, Value&& v) {
        auto insertRes = m.map.try_emplace(k, std::move(v));
        if (insertRes.second) {
            m.orderedKeys.push_back(std::move(k));
        }
    }
    static void addToMapIfNotContained(MapOrderedKeysPair& m, const std::string& k, const Value& v) {
        auto insertRes = m.map.try_emplace(k, v);
        if (insertRes.second) {
            m.orderedKeys.push_back(k);
        }
    }

    template <typename Func>
    static void forEachMap(MapOrderedKeysPair&& m, Func&& f) {
        for (auto&& k : std::move(m.orderedKeys)) {
            auto v = std::move(m.map.at(k));
            f(std::move(k), std::move(v));
        }
    }
    template <typename Func>
    static void forEachMap(const MapOrderedKeysPair& m, Func&& f) {
        for (const auto& k : m.orderedKeys) {
            f(k, m.map.at(k));
        }
    }
    template <typename Func>
    static void forEachMap(Value&& m, Func&& f) {
        // Do not move, don't know if value is shared...
        Value keys = m.keys();
        const auto size = keys.size();
        for (unsigned int i = 0; i < size; ++i) {
            auto key = keys[i];
            auto val = m[key];
            f(std::move(key), std::move(val));
        }
    }
    template <typename Func>
    static void forEachMap(const Value& m, Func&& f) {
        Value keys = m.keys();
        const auto size = keys.size();
        for (unsigned int i = 0; i < size; ++i) {
            auto key = keys[i];
            f(key, m[key]);
        }
    }


    static ValueList from(ObjectValueTag<ObjectValueType::List>) {
        return ValueList{};
    };

    static void addToList(ValueList& l, Value&& v) {
        l.push_back(std::move(v));
    }
    static void addToList(ValueList& l, const Value& v) {
        l.push_back(std::move(v));
    }

    template <typename Func>
    static void forEachList(ValueList&& l, Func&& f) {
        for (auto&& v : l) {
            f(std::move(v));
        }
    }
    template <typename Func>
    static void forEachList(const ValueList& l, Func&& f) {
        for (const auto& v : l) {
            f(v);
        }
    }
    
    template <typename Func>
    static void forEachList(Value&& l, Func&& f) {
        const auto size = l.size();
        for (unsigned int i = 0; i < size; ++i) {
            // Do not move, don't know if value is shared...
            f(l[i]);
        }
    }
    template <typename Func>
    static void forEachList(const Value& l, Func&& f) {
        const auto size = l.size();
        for (unsigned int i = 0; i < size; ++i) {
            f(l[i]);
        }
    }

    static Value toValue(Value&& v) {
        return std::move(v);
    }
    static Value toValue(MapOrderedKeysPair&& v) {
        return Value::makeOrderedMap(std::move(v.map), std::move(v.orderedKeys));
    }
    static Value toValue(ValueList&& v) {
        return Value::makeList(std::move(v));
    }
    template <typename T>
    static Value toValue(T&& t) {
        return Value(std::forward<T>(t));
    }

    template <typename Func>
    static decltype(auto) visit(Value&& v, Func&& f) {
        if (v.isList()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::List>{}, std::move(v));
        }
        else if (v.isMap()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Map>{}, std::move(v));
        }
        else if (v.isNumber()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Number>{}, std::move(v));
        }
        else if (v.isDouble()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Number>{}, std::move(v));
        }
        else if (v.isBool()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Bool>{}, std::move(v));
        }
        else if (v.isString()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::String>{}, std::move(v));
        }
        else if (v.isNil()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Null>{}, std::move(v));
        }
        else {
            throw SeriousBug("eckit::Value seems to contain a content that has nott been created with this ObjectBuilder.");
        }
    }

    template <typename Func>
    static decltype(auto) visit(const Value& v, Func&& f) {
        if (v.isList()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::List>{}, v);
        }
        else if (v.isMap()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Map>{}, v);
        }
        else if (v.isNumber()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Number>{}, v);
        }
        else if (v.isDouble()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Number>{}, v);
        }
        else if (v.isBool()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Bool>{}, v);
        }
        else if (v.isString()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::String>{}, v);
        }
        else if (v.isNil()) {
            return std::forward<Func>(f)(ObjectValueTag<ObjectValueType::Null>{}, v);
        }
        else {
            throw SeriousBug("eckit::Value seems to contain a content that has nott been created with this ObjectBuilder.");
        }
    }
};

}  // namespace parser

using ObjectParser = GenericObjectParser<parser::ValueBuilder>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
