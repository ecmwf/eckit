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


template <typename As>
bool __get_a(const ValueMap& map, const std::string& name, As& value) {
    if (auto it = map.find(name); it != map.end()) {
        value = it->second.as<As>();
        return true;
    }

    return false;
}


// FIXME there are conversion issues with the above (?) so we use this for now
template <typename As>
bool __get_b(const ValueMap& map, const std::string& name, As& value) {
    return false;
}


}  // namespace


bool Valued::has(const std::string& name) const {
    return map_.find(name) != map_.end();
}


bool Valued::get(const std::string& name, std::string& value) const {
    return __get_a(map_, name, value);
}


bool Valued::get(const std::string& name, bool& value) const {
    return __get_a(map_, name, value);
}


bool Valued::get(const std::string& name, int& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, long& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, long long& value) const {
    return __get_a(map_, name, value);
}


bool Valued::get(const std::string& name, std::size_t& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, float& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, double& value) const {
    return __get_a(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<int>& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<long>& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<long long>& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<std::size_t>& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<float>& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<double>& value) const {
    return __get_b(map_, name, value);
}


bool Valued::get(const std::string& name, std::vector<std::string>& value) const {
    return __get_b(map_, name, value);
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
