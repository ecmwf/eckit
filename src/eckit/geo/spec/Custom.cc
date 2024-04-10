/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/spec/Custom.h"

#include <algorithm>
#include <cctype>
#include <type_traits>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/log/JSON.h"
#include "eckit/value/Content.h"  // for ValueList, ValueMap
#include "eckit/value/Value.h"


namespace eckit::geo::spec {


namespace {


template <typename From, typename To>
bool __get_s(const From& from, To& to) {
    to = static_cast<To>(from);
    return true;
}


template <typename From>
bool __get_s(const From& from, std::string& to) {
    to = std::to_string(from);
    return true;
}


template <typename From>
bool __get_s(const From& from, From& to) {
    to = from;
    return true;
}


template <typename From, typename To>
bool __get_v(const std::vector<From>& from, std::vector<To>& to) {
    to.clear();
    for (const auto& f : from) {
        to.emplace_back(static_cast<To>(f));
    }
    return true;
}


template <typename From>
bool __get_v(const std::vector<From>& from, std::vector<From>& to) {
    to = from;
    return true;
}


template <typename T>
bool __get_s_integral(const Custom::container_type& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<int>(v)         ? __get_s(std::get<int>(v), value)
               : std::holds_alternative<long>(v)      ? __get_s(std::get<long>(v), value)
               : std::holds_alternative<long long>(v) ? __get_s(std::get<long long>(v), value)
               : std::holds_alternative<size_t>(v)    ? __get_s(std::get<size_t>(v), value)
                                                      : false;
    }
    return false;
}


template <typename T>
bool __get_s_real(const Custom::container_type& map, const std::string& name, T& value) {
    if (__get_s_integral(map, name, value)) {
        return true;
    }

    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<float>(v)    ? __get_s(std::get<float>(v), value)
               : std::holds_alternative<double>(v) ? __get_s(std::get<double>(v), value)
                                                   : false;
    }
    return false;
}


template <typename T>
bool __get_v_integral(const Custom::container_type& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<std::vector<int>>(v)         ? __get_v(std::get<std::vector<int>>(v), value)
               : std::holds_alternative<std::vector<long>>(v)      ? __get_v(std::get<std::vector<long>>(v), value)
               : std::holds_alternative<std::vector<long long>>(v) ? __get_v(std::get<std::vector<long long>>(v), value)
               : std::holds_alternative<std::vector<size_t>>(v)    ? __get_v(std::get<std::vector<size_t>>(v), value)
                                                                   : false;
    }
    return false;
}


template <typename T>
bool __get_v_real(const Custom::container_type& map, const std::string& name, T& value) {
    if (__get_v_integral(map, name, value)) {
        return true;
    }

    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<std::vector<float>>(v)    ? __get_v(std::get<std::vector<float>>(v), value)
               : std::holds_alternative<std::vector<double>>(v) ? __get_v(std::get<std::vector<double>>(v), value)
                                                                : false;
    }
    return false;
}


template <typename T>
Custom::value_type __from_value(const Value& value) {
    T to;
    fromValue(to, value);
    return {to};
}


JSON& operator<<(JSON& out, const Custom::value_type& value) {
    std::visit([&](const auto& arg) { out << arg; }, value);
    return out;
}


}  // namespace


Custom::key_type::key_type(const std::string& s) :
    std::string{s} {
    std::transform(begin(), end(), begin(), [](unsigned char c) -> unsigned char { return std::tolower(c); });
}


Custom::Custom(const Custom::container_type& map) :
    map_(map) {}


Custom::Custom(Custom::container_type&& map) :
    map_(map) {}


Custom::Custom(const Value& value) {
    auto scalar = [](const Value& value) -> value_type {
        return value.isNumber()   ? value_type(static_cast<int>(value))
               : value.isDouble() ? value_type(static_cast<double>(value))
               : value.isString() ? static_cast<std::string>(value)
                                  : throw BadValue(value);
    };

    auto vector = [](const Value& value) -> value_type {
        const ValueList list(value);
        ASSERT(!list.empty());
        return list.front().isNumber()   ? value_type(std::vector<int>(list.begin(), list.end()))
               : list.front().isDouble() ? value_type(std::vector<double>(list.begin(), list.end()))
               : list.front().isString() ? std::vector<std::string>(list.begin(), list.end())
                                         : throw BadValue(value);
    };

    ASSERT(value.isMap());
    for (const auto& [key, value] : static_cast<ValueMap>(value)) {
        const std::string name = key;
        map_[name]             = value.isList() ? vector(value) : scalar(value);
    }
}


Custom::Custom(const Custom& custom) :
    Custom(custom.map_) {}


Custom::Custom(Custom&& custom) :
    Custom(custom.map_) {}


Custom& Custom::operator=(Custom&& custom) {
    map_.swap(custom.map_);
    return *this;
}


bool Custom::operator==(const Custom& other) const {
    auto custom_value_equal = [](const auto& a, const auto& b) -> bool {
        if constexpr (std::is_same_v<decltype(a), decltype(b)>) {
            return a == b;
        }
        else {
            return false;
        }
    };

    return std::all_of(map_.begin(), map_.end(), [&](const auto& _a) {
        auto _b = other.map_.find(_a.first);
        return _b != other.map_.end() && custom_value_equal(_a.second, _b->second);
    });
}


Custom& Custom::operator=(const Custom& custom) {
    map_ = custom.map_;
    return *this;
}


void Custom::set(const std::string& name, const std::string& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, bool value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, int value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, long value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, long long value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, size_t value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, float value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, double value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<int>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<long>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<long long>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<size_t>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<float>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<double>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& name, const std::vector<std::string>& value) {
    map_[name] = value;
}


void Custom::set(const std::string& key, const Value& value) {
    using number_type = pl_type::value_type;

    auto list_of = [](const ValueList& list, auto pred) { return std::all_of(list.begin(), list.end(), pred); };

    auto val = value.isList() && list_of(value, [](const Value& v) { return v.isDouble(); })
                   ? __from_value<std::vector<double>>(value)
               : value.isList() && list_of(value, [](const Value& v) { return v.isNumber(); })
                   ? __from_value<std::vector<number_type>>(value)
               : value.isList()   ? __from_value<std::vector<std::string>>(value)
               : value.isDouble() ? __from_value<double>(value)
               : value.isNumber() ? __from_value<number_type>(value)
                                  : __from_value<std::string>(value);

    std::visit([&](const auto& val) { set(key, val); }, val);
}


bool Custom::has(const std::string& name) const {
    return map_.find(name) != map_.cend();
}


bool Custom::get(const std::string& name, std::string& value) const {
    if (auto it = map_.find(name); it != map_.cend() /*&& )*/) {
        if (std::holds_alternative<std::string>(it->second)) {
            value = std::get<std::string>(it->second);
            return true;
        }

        return __get_s_real(map_, name, value);
    }
    return false;
}


bool Custom::get(const std::string& name, bool& value) const {
    if (auto it = map_.find(name); it != map_.cend()) {
        if (std::holds_alternative<bool>(it->second)) {
            value = std::get<bool>(it->second);
            return true;
        }

        if (int i = 0; __get_s_integral<int>(map_, name, i)) {
            value = i != 0;
            return true;
        }
    }
    return false;
}


bool Custom::get(const std::string& name, int& value) const {
    return __get_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, long& value) const {
    return __get_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, long long& value) const {
    return __get_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, size_t& value) const {
    return __get_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, float& value) const {
    return __get_s_real(map_, name, value);
}


bool Custom::get(const std::string& name, double& value) const {
    return __get_s_real(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<int>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<long>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<long long>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<size_t>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<float>& value) const {
    return __get_v_real(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<double>& value) const {
    return __get_v_real(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<std::string>& value) const {
    if (auto it = map_.find(name); it != map_.cend()) {
        if (std::holds_alternative<std::vector<std::string>>(it->second)) {
            value = std::get<std::vector<std::string>>(it->second);
            return true;
        }
    }
    return false;
}


void Custom::json(JSON& j) const {
    j.startObject();
    for (const auto& nv : map_) {
        j << nv.first << nv.second;
    }
    j.endObject();
}


}  // namespace eckit::geo::spec
