/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/DynamicConfiguration.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


namespace {


const LocalConfiguration __empty_configuration;
const eckit::Value __empty_root;


}  // namespace


DynamicConfiguration::DynamicConfiguration() :
    DynamicConfiguration(__empty_configuration) {}


DynamicConfiguration::DynamicConfiguration(const Configuration& passive) :
    Configuration(__empty_root), passive_(passive) {}


void DynamicConfiguration::set(const std::string& name, std::string& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, bool& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, int& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, long& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, long long& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::size_t& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, float& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, double& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<int>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<long>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<long long>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<std::size_t>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<float>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<double>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::set(const std::string& name, std::vector<std::string>& value) {
    __set(*this, name, value);
}


void DynamicConfiguration::hide(const std::string& name) {
    hide_.insert(name);
}


void DynamicConfiguration::unhide(const std::string& name) {
    hide_.erase(name);
}


bool DynamicConfiguration::has(const std::string& name) const {
    return !hide_.contains(name) && (active_.has(name) || passive_.has(name));
}


bool DynamicConfiguration::get(const std::string& name, std::string& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, bool& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, int& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, long& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, long long& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::size_t& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, float& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, double& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<int>& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<long>& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<long long>& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<std::size_t>& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<float>& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<double>& value) const {
    return __get(*this, name, value);
}


bool DynamicConfiguration::get(const std::string& name, std::vector<std::string>& value) const {
    return __get(*this, name, value);
}


void DynamicConfiguration::print(std::ostream& out) const {
    out << "DynamicConfiguration[active=" << active_ << ",passive=" << passive_ << "]";
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
