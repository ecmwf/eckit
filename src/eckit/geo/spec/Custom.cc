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
#include <sstream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/log/JSON.h"
#include "eckit/value/Content.h"  // for ValueList, ValueMap
#include "eckit/value/Value.h"


namespace eckit::geo::spec {


namespace {


constexpr int STREAM_PRECISION = 15;


template <typename From, typename To>
bool get_t_s(const From& from, To& to) {
    to = static_cast<To>(from);
    return true;
}


template <typename From>
bool get_t_s(const From& from, std::string& to) {
    to = std::to_string(from);
    return true;
}


template <typename From>
bool get_t_s(const From& from, From& to) {
    to = from;
    return true;
}


template <typename From, typename To>
bool get_t_v(const std::vector<From>& from, std::vector<To>& to) {
    to.clear();
    for (const auto& f : from) {
        to.emplace_back(static_cast<To>(f));
    }
    return true;
}


template <typename From>
bool get_t_v(const std::vector<From>& from, std::vector<std::string>& to) {
    to.clear();
    for (const auto& f : from) {
        to.emplace_back(std::to_string(f));
    }
    return true;
}


template <typename T>
bool get_t_s_integral(const Custom::container_type& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<int>(v)         ? get_t_s(std::get<int>(v), value)
               : std::holds_alternative<long>(v)      ? get_t_s(std::get<long>(v), value)
               : std::holds_alternative<long long>(v) ? get_t_s(std::get<long long>(v), value)
               : std::holds_alternative<size_t>(v)    ? get_t_s(std::get<size_t>(v), value)
                                                      : false;
    }
    return false;
}


template <typename T>
bool get_t_s_real(const Custom::container_type& map, const std::string& name, T& value) {
    if (get_t_s_integral(map, name, value)) {
        return true;
    }

    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<float>(v)    ? get_t_s(std::get<float>(v), value)
               : std::holds_alternative<double>(v) ? get_t_s(std::get<double>(v), value)
                                                   : false;
    }
    return false;
}


template <typename T>
bool get_t_v_integral(const Custom::container_type& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<std::vector<int>>(v)         ? get_t_v(std::get<std::vector<int>>(v), value)
               : std::holds_alternative<std::vector<long>>(v)      ? get_t_v(std::get<std::vector<long>>(v), value)
               : std::holds_alternative<std::vector<long long>>(v) ? get_t_v(std::get<std::vector<long long>>(v), value)
               : std::holds_alternative<std::vector<size_t>>(v)    ? get_t_v(std::get<std::vector<size_t>>(v), value)
                                                                   : false;
    }
    return false;
}


template <typename T>
bool get_t_v_real(const Custom::container_type& map, const std::string& name, T& value) {
    if (get_t_v_integral(map, name, value)) {
        return true;
    }

    if (auto it = map.find(name); it != map.cend()) {
        const auto& v = it->second;
        return std::holds_alternative<std::vector<float>>(v)    ? get_t_v(std::get<std::vector<float>>(v), value)
               : std::holds_alternative<std::vector<double>>(v) ? get_t_v(std::get<std::vector<double>>(v), value)
                                                                : false;
    }
    return false;
}


template <typename T>
Custom::value_type from_value_t(const Value& value) {
    T to;
    fromValue(to, value);
    return {to};
}


void sanitise(Custom::container_type& container) {
    std::for_each(container.begin(), container.end(), [](auto& p) {
        if (auto& value = p.second; std::holds_alternative<const char*>(value)) {
            value = std::string{std::get<const char*>(value)};
        }
        else if (std::holds_alternative<Custom::custom_ptr>(value)) {
            ASSERT(std::get<Custom::custom_ptr>(value));
        }
    });
}


}  // namespace


Custom::key_type::key_type(const std::string& s) : std::string{s} {
    std::transform(begin(), end(), begin(), [](unsigned char c) -> unsigned char { return std::tolower(c); });
}


Custom::Custom(std::initializer_list<container_type::value_type> init) : map_(init) {
    sanitise(map_);
}


Custom::Custom(const Custom::container_type& map) : map_(map) {
    sanitise(map_);
}


Custom::Custom(Custom::container_type&& map) : map_(map) {
    sanitise(map_);
}


Custom* Custom::make_from_value(const Value& value) {
    ASSERT(value.isMap());

    auto scalar = [](const Value& value) -> value_type {
        return value.isNumber()   ? value_type(static_cast<int>(value))
               : value.isDouble() ? value_type(static_cast<double>(value))
               : value.isString() ? static_cast<std::string>(value)
                                  : throw BadValue(value, Here());
    };

    auto vector = [](const Value& value) -> value_type {
        const auto list(value.as<ValueList>());
        ASSERT(!list.empty());

        return list.front().isNumber()   ? value_type(std::vector<int>(list.begin(), list.end()))
               : list.front().isDouble() ? value_type(std::vector<double>(list.begin(), list.end()))
               : list.front().isString() ? std::vector<std::string>(list.begin(), list.end())
                                         : throw BadValue(value, Here());
    };

    Custom::container_type container;
    for (const auto& [key, value] : static_cast<ValueMap>(value)) {
        const std::string name = key;

        container[name] = value.isMap()    ? custom_ptr(Custom::make_from_value(value))
                          : value.isList() ? vector(value)
                                           : scalar(value);
    }

    return new Custom(std::move(container));
}


bool Custom::operator==(const Custom& other) const {
    auto custom_value_equal
        = [](const Custom& ca, const Custom& cb, const Custom::key_type& name, const auto& type_instance) -> bool {
        auto a = type_instance;
        auto b = type_instance;
        return ca.get(name, a) && cb.get(name, b) && a == b;
    };

    // check every local key exists in other and is convertible to an equal value
    return std::all_of(map_.begin(), map_.end(), [&](const auto& _a) {
        const auto& name = _a.first;
        auto _b          = other.map_.find(name);
        return _b != other.map_.end()
               && (custom_value_equal(*this, other, name, long{})
                   || custom_value_equal(*this, other, name, std::vector<long>{})
                   || custom_value_equal(*this, other, name, double{})
                   || custom_value_equal(*this, other, name, std::vector<double>{})
                   || custom_value_equal(*this, other, name, std::string{})
                   || custom_value_equal(*this, other, name, std::vector<std::string>{}));
    });
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
                   ? from_value_t<std::vector<double>>(value)
               : value.isList() && list_of(value, [](const Value& v) { return v.isNumber(); })
                   ? from_value_t<std::vector<number_type>>(value)
               : value.isList()   ? from_value_t<std::vector<std::string>>(value)
               : value.isDouble() ? from_value_t<double>(value)
               : value.isNumber() ? from_value_t<number_type>(value)
                                  : from_value_t<std::string>(value);

    std::visit([&](const auto& val) { set(key, val); }, val);
}


void Custom::set(const std::string& name, Custom* value) {
    ASSERT(value != nullptr);
    map_[name] = custom_ptr(value);
}


bool Custom::has_custom(const std::string& name) const {
    auto it = map_.find(name);
    return it != map_.cend() && std::holds_alternative<custom_ptr>(it->second);
}


const Custom::custom_ptr& Custom::custom(const std::string& name) const {
    if (auto it = map_.find(name); it != map_.cend()) {
        if (std::holds_alternative<custom_ptr>(it->second)) {
            const auto& value = std::get<custom_ptr>(it->second);
            ASSERT(value);

            return value;
        }
    }

    throw SpecNotFound("Custom::get(" + name + ") -> custom_type& ", Here());
}


void Custom::set(const std::string& name, const custom_ptr& value) {
    ASSERT(value);
    map_[name] = value;
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

        return get_t_s_real(map_, name, value);
    }
    return false;
}


bool Custom::get(const std::string& name, bool& value) const {
    if (auto it = map_.find(name); it != map_.cend()) {
        if (std::holds_alternative<bool>(it->second)) {
            value = std::get<bool>(it->second);
            return true;
        }

        if (int i = 0; get_t_s_integral<int>(map_, name, i)) {
            value = i != 0;
            return true;
        }
    }
    return false;
}


bool Custom::get(const std::string& name, int& value) const {
    return get_t_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, long& value) const {
    return get_t_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, long long& value) const {
    return get_t_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, size_t& value) const {
    return get_t_s_integral(map_, name, value);
}


bool Custom::get(const std::string& name, float& value) const {
    return get_t_s_real(map_, name, value);
}


bool Custom::get(const std::string& name, double& value) const {
    return get_t_s_real(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<int>& value) const {
    return get_t_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<long>& value) const {
    return get_t_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<long long>& value) const {
    return get_t_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<size_t>& value) const {
    return get_t_v_integral(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<float>& value) const {
    return get_t_v_real(map_, name, value);
}


bool Custom::get(const std::string& name, std::vector<double>& value) const {
    return get_t_v_real(map_, name, value);
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
    j.precision(STREAM_PRECISION);
    for (const auto& [key, value] : map_) {
        j << key;
        std::visit([&](const auto& arg) { j << arg; }, value);
    }
    j.endObject();
}


JSON& operator<<(JSON& j, const Custom::custom_ptr& value) {
    ASSERT(value);
    j.startObject();
    for (const auto& [key, value] : value->container()) {
        j << key;
        std::visit([&](const auto& arg) { j << arg; }, value);
    }

    j.endObject();
    return j;
}


template <typename T>
struct is_vector : std::false_type {};


template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};


template <typename T>
constexpr bool is_vector_v = is_vector<T>::value;


std::string to_string(const Custom::value_type& value) {
    return std::visit(
        [&](const auto& arg) {
            std::ostringstream str;
            str.precision(STREAM_PRECISION);
            str << arg;
            return str.str();
        },
        value);
}


}  // namespace eckit::geo::spec
