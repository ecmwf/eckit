/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/MappedConfiguration.h"

#include <ostream>

#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


namespace {


const eckit::Value __empty_root;


template <typename T>
bool __get(const MappedConfiguration::container_type& map, const std::string& name, T& value) {
    if (auto it = map.find(name); it != map.cend()) {
        value = std::get<T>(it->second);
        return true;
    }
    return false;
}


std::ostream& operator<<(std::ostream& out, const MappedConfiguration::value_type& v) {
    std::visit([&](auto&& arg) { out << arg; }, v);
    return out;
}


}  // namespace


MappedConfiguration::MappedConfiguration(const MappedConfiguration::container_type& map) :
    Configuration(__empty_root),
    map_(map) {}


MappedConfiguration::MappedConfiguration(MappedConfiguration::container_type&& map) :
    Configuration(__empty_root),
    map_(map) {}


MappedConfiguration::MappedConfiguration(const MappedConfiguration& config) :
    MappedConfiguration(config.map_) {}


MappedConfiguration::MappedConfiguration(MappedConfiguration&& config) :
    MappedConfiguration(config.map_) {}


MappedConfiguration& MappedConfiguration::operator=(MappedConfiguration&& config) {
    map_.swap(config.map_);
    return *this;
}


MappedConfiguration& MappedConfiguration::operator=(const MappedConfiguration& config) {
    map_ = config.map_;
    return *this;
}


void MappedConfiguration::set(const std::string& name, const std::string& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, bool value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, int value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, long value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, long long value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, std::size_t value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, float value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, double value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<int>& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<long>& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<long long>& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<std::size_t>& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<float>& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<double>& value) {
    map_[name] = value;
}


void MappedConfiguration::set(const std::string& name, const std::vector<std::string>& value) {
    map_[name] = value;
}


bool MappedConfiguration::has(const std::string& name) const {
    return map_.find(name) != map_.cend();
}


bool MappedConfiguration::get(const std::string& name, std::string& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, bool& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, int& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, long& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, long long& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::size_t& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, float& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, double& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<int>& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<long>& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<long long>& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<std::size_t>& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<float>& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<double>& value) const {
    return __get(map_, name, value);
}


bool MappedConfiguration::get(const std::string& name, std::vector<std::string>& value) const {
    return __get(map_, name, value);
}


void MappedConfiguration::print(std::ostream& out) const {
    out << "MappedConfiguration[";

    const auto* sep = "";
    for (const auto& nv : map_) {
        out << sep << nv.first << ": " << nv.second;
        sep = ", ";
    }

    out << "]";
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
