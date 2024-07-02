/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Spec.h"

#include <sstream>

#include "eckit/log/JSON.h"


namespace eckit::geo {


template <typename T>
static T _get_d(const Spec& spec, const std::string& name, const T& _default) {
    T value{_default};
    spec.get(name, value);
    return value;
}


template <typename T>
static T _get_t(const Spec& spec, const std::string& name) {
    T value{};
    return spec.get(name, value) ? value : throw SpecNotFound(name, Here());
}


SpecNotFound::SpecNotFound(const std::string& name, const CodeLocation& loc) : Exception(loc) {
    reason("SpecNotFound: [" + name + "], in " + loc.asString());
}


std::string Spec::get_string(const std::string& name) const {
    return _get_t<std::string>(*this, name);
}


bool Spec::get_bool(const std::string& name) const {
    return _get_t<bool>(*this, name);
}


int Spec::get_int(const std::string& name) const {
    return _get_t<int>(*this, name);
}


long Spec::get_long(const std::string& name) const {
    return _get_t<long>(*this, name);
}


size_t Spec::get_unsigned(const std::string& name) const {
    return _get_t<size_t>(*this, name);
}


double Spec::get_double(const std::string& name) const {
    return _get_t<double>(*this, name);
}


std::vector<long> Spec::get_long_vector(const std::string& name) const {
    return _get_t<std::vector<long>>(*this, name);
}


std::vector<size_t> Spec::get_unsigned_vector(const std::string& name) const {
    return _get_t<std::vector<size_t>>(*this, name);
}


std::vector<double> Spec::get_double_vector(const std::string& name) const {
    return _get_t<std::vector<double>>(*this, name);
}


std::string Spec::get_string(const std::string& name, const std::string& _default) const {
    return _get_d(*this, name, _default);
}


bool Spec::get_bool(const std::string& name, const bool& _default) const {
    return _get_d(*this, name, _default);
}


int Spec::get_int(const std::string& name, const int& _default) const {
    return _get_d(*this, name, _default);
}


long Spec::get_long(const std::string& name, const long& _default) const {
    return _get_d(*this, name, _default);
}


size_t Spec::get_unsigned(const std::string& name, const size_t& _default) const {
    return _get_d(*this, name, _default);
}


double Spec::get_double(const std::string& name, const double& _default) const {
    return _get_d(*this, name, _default);
}


std::vector<long> Spec::get_long_vector(const std::string& name, const std::vector<long>& _default) const {
    return _get_d(*this, name, _default);
}


std::vector<size_t> Spec::get_unsigned_vector(const std::string& name, const std::vector<size_t>& _default) const {
    return _get_d(*this, name, _default);
}


std::vector<double> Spec::get_double_vector(const std::string& name, const std::vector<double>& _default) const {
    return _get_d(*this, name, _default);
}


std::string Spec::str() const {
    std::ostringstream str;
    JSON j(str);
    json(j);
    return str.str();
}


void Spec::print(std::ostream& out) const {
    JSON j(out);
    json(j);
}


}  // namespace eckit::geo
