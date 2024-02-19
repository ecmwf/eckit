/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/spec/Valued.h"

#include "eckit/log/JSON.h"


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
bool __get_s_integral(const ValueMap& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend() && it->second.isNumber()) {
        value = static_cast<int>(it->second);  // (range constraint)
        return true;
    }
    return false;
}


template <typename T>
bool __get_s_real(const ValueMap& map, const std::string& name, T& value) {
    if (__get_s_integral(map, name, value)) {
        return true;
    }

    if (auto it = map.find(name); it != map.cend() && it->second.isDouble()) {
        value = static_cast<double>(it->second);  // (range constraint)
        return true;
    }
    return false;
}


template <typename T>
bool __get_v_integral(const ValueMap& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend() && it->second.isList()) {
        if (ValueList l(it->second); !l.empty() && l.front().isNumber()) {
            value.clear();
            for (const auto& v : l) {
                value.emplace_back(static_cast<int>(v));
            }
            return true;
        }
    }
    return false;
}


template <typename T>
bool __get_v_real(const ValueMap& map, const std::string& name, T& value) {
    if (__get_v_integral(map, name, value)) {
        return true;
    }

    if (auto it = map.find(name); it != map.cend() && it->second.isList()) {
        if (ValueList l(it->second); !l.empty() && l.front().isDouble()) {
            value.clear();
            for (const auto& v : l) {
                value.emplace_back(static_cast<double>(v));
            }
            return true;
        }
    }
    return false;
}


}  // namespace


bool Valued::has(const std::string& name) const {
    return map_.find(name) != map_.end();
}


bool Valued::get(const std::string& name, std::string& value) const {
    if (auto it = map_.find(name); it != map_.cend()) {
        if (it->second.isString()) {
            value = static_cast<std::string>(it->second);
            return true;
        }

        return __get_s_real(map_, name, value);
    }
    return false;
}


bool Valued::get(const std::string& name, bool& value) const {
    if (auto it = map_.find(name); it != map_.cend()) {
        if (it->second.isBool()) {
            value = it->second;
            return true;
        }

        if (int i = 0; __get_s_integral<int>(map_, name, i)) {
            value = i != 0;
            return true;
        }
    }
    return false;
}


bool Valued::get(const std::string& name, int& value) const {
    return __get_s_integral(map_, name, value);
}


bool Valued::get(const std::string& name, long& value) const {
    return __get_s_integral(map_, name, value);
}


bool Valued::get(const std::string& name, long long& value) const {
    return __get_s_integral(map_, name, value);
}


bool Valued::get(const std::string& name, std::size_t& value) const {
    return __get_s_integral(map_, name, value);
}


bool Valued::get(const std::string& name, float& value) const {
    return __get_s_real(map_, name, value);
}


bool Valued::get(const std::string& name, double& value) const {
    return __get_s_real(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<int>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<long>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<long long>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<std::size_t>& value) const {
    return __get_v_integral(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<float>& value) const {
    return __get_v_real(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<double>& value) const {
    return __get_v_real(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<std::string>& value) const {
    if (auto it = map_.find(name); it != map_.cend() && it->second.isList()) {
        if (ValueList l(it->second); !l.empty() && l.front().isString()) {
            value.clear();
            for (const auto& v : l) {
                value.emplace_back(v);
            }
            return true;
        }
    }
    return false;
}


void Valued::json(JSON& j) const {
    j.startObject();
    for (auto& [key, value] : map_) {
        j << key;
        j << value;
    }
    j.endObject();
}


}  // namespace eckit::geo::spec
