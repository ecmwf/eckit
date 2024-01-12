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

#include "eckit/log/JSON.h"


namespace eckit::geo {


template <typename T>
static T _get(const Spec& spec, const std::string& name, const T& _default) {
    T value{_default};
    spec.get(name, value);
    return value;
}


SpecNotFound::SpecNotFound(const std::string& name) {
    std::ostringstream s;
    s << "SpecNotFound: [" << name << "]";
    reason(s.str());
}


bool Spec::get_bool(const std::string& name) const {
    ASSERT(has(name));
    return _get(*this, name, false);
}


int Spec::get_int(const std::string& name) const {
    ASSERT(has(name));
    return _get<int>(*this, name, 0);
}


long Spec::get_long(const std::string& name) const {
    ASSERT(has(name));
    return _get<long>(*this, name, 0);
}


std::size_t Spec::get_unsigned(const std::string& name) const {
    ASSERT(has(name));
    return _get<std::size_t>(*this, name, 0);
}


double Spec::get_double(const std::string& name) const {
    ASSERT(has(name));
    return _get<double>(*this, name, 0);
}


std::string Spec::get_string(const std::string& name) const {
    ASSERT(has(name));
    return _get<std::string>(*this, name, {});
}


std::vector<long> Spec::get_long_vector(const std::string& name) const {
    ASSERT(has(name));
    return _get<std::vector<long>>(*this, name, {});
}


bool Spec::get_bool(const std::string& name, const bool& _default) const {
    return _get(*this, name, _default);
}


int Spec::get_int(const std::string& name, const int& _default) const {
    return _get(*this, name, _default);
}


long Spec::get_long(const std::string& name, const long& _default) const {
    return _get(*this, name, _default);
}


std::size_t Spec::get_unsigned(const std::string& name, const std::size_t& _default) const {
    return _get(*this, name, _default);
}


double Spec::get_double(const std::string& name, const double& _default) const {
    return _get(*this, name, _default);
}


std::string Spec::get_string(const std::string& name, const std::string& _default) const {
    return _get(*this, name, _default);
}


std::vector<long> Spec::get_long_vector(const std::string& name, const std::vector<long>& _default) const {
    return _get(*this, name, _default);
}


void Spec::print(std::ostream& out) const {
    JSON j(out);
    json(j);
}


}  // namespace eckit::geo
