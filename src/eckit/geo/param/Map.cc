/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "grit/param/Map.h"

#include <cstddef>

#include "grit/exception.h"


namespace grit::param {


Map::Map(const container_type& other) : map_(other) {}


bool Map::has(const key_type& key) const {
    return map_.find(key) != map_.end();
}


template <typename T>
bool __get(const Map::container_type& map, std::size_t idx, const Map::key_type& key, T& value) {
    if (auto it = map.find(key); it != map.end()) {
        ASSERT(it->second.index() == idx);
        value = std::get<T>(it->second);
        return true;
    }

    return false;
}


void Map::set(const key_type& key, const bool& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const int& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const unsigned int& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const long& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const unsigned long& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const float& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const double& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::string& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<bool>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<int>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<unsigned int>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<long>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<unsigned long>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<float>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<double>& value) {
    map_[key] = value;
}


void Map::set(const key_type& key, const std::vector<std::string>& value) {
    map_[key] = value;
}


bool Map::get(const key_type& key, bool& value) const {
    return __get<bool>(map_, 0, key, value);
}


bool Map::get(const key_type& key, int& value) const {
    return __get<int>(map_, 1, key, value);
}


bool Map::get(const key_type& key, unsigned int& value) const {
    return __get<unsigned int>(map_, 2, key, value);
}


bool Map::get(const key_type& key, long& value) const {
    return __get<long>(map_, 3, key, value);
}


bool Map::get(const key_type& key, unsigned long& value) const {
    return __get<unsigned long>(map_, 4, key, value);
}


bool Map::get(const key_type& key, float& value) const {
    return __get<float>(map_, 5, key, value);
}


bool Map::get(const key_type& key, double& value) const {
    return __get<double>(map_, 6, key, value);
}


bool Map::get(const key_type& key, std::string& value) const {
    return __get<std::string>(map_, 7, key, value);
}


bool Map::get(const key_type& key, std::vector<bool>& value) const {
    return __get<std::vector<bool>>(map_, 8, key, value);
}


bool Map::get(const key_type& key, std::vector<int>& value) const {
    return __get<std::vector<int>>(map_, 9, key, value);
}


bool Map::get(const key_type& key, std::vector<unsigned int>& value) const {
    return __get<std::vector<unsigned int>>(map_, 0, key, value);
}


bool Map::get(const key_type& key, std::vector<long>& value) const {
    return __get<std::vector<long>>(map_, 11, key, value);
}


bool Map::get(const key_type& key, std::vector<unsigned long>& value) const {
    return __get<std::vector<unsigned long>>(map_, 12, key, value);
}


bool Map::get(const key_type& key, std::vector<float>& value) const {
    return __get<std::vector<float>>(map_, 13, key, value);
}


bool Map::get(const key_type& key, std::vector<double>& value) const {
    return __get<std::vector<double>>(map_, 14, key, value);
}


bool Map::get(const key_type& key, std::vector<std::string>& value) const {
    return __get<std::vector<std::string>>(map_, 15, key, value);
}


}  // namespace grit::param
