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


#include "eckit/geo/Parametrisation.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo {


namespace {


template <typename T>
T __get(const Parametrisation& param, const Parametrisation::key_type& key) {
    auto value = T();
    ASSERT(param.get(key, value));
    return value;
}


}  // namespace


bool Parametrisation::get_bool(const Parametrisation::key_type& key) const {
    return __get<bool>(*this, key);
}


int Parametrisation::get_int(const Parametrisation::key_type& key) const {
    return __get<int>(*this, key);
}


unsigned int Parametrisation::get_unsigned_int(const Parametrisation::key_type& key) const {
    return __get<unsigned int>(*this, key);
}


long Parametrisation::get_long(const Parametrisation::key_type& key) const {
    return __get<long>(*this, key);
}


unsigned long Parametrisation::get_unsigned_long(const Parametrisation::key_type& key) const {
    return __get<unsigned long>(*this, key);
}


float Parametrisation::get_float(const Parametrisation::key_type& key) const {
    return __get<float>(*this, key);
}


double Parametrisation::get_double(const Parametrisation::key_type& key) const {
    return __get<double>(*this, key);
}


std::string Parametrisation::get_string(const Parametrisation::key_type& key) const {
    return __get<std::string>(*this, key);
}


std::vector<bool> Parametrisation::get_vector_bool(const Parametrisation::key_type& key) const {
    return __get<std::vector<bool>>(*this, key);
}


std::vector<int> Parametrisation::get_vector_int(const Parametrisation::key_type& key) const {
    return __get<std::vector<int>>(*this, key);
}


std::vector<unsigned int> Parametrisation::get_vector_unsigned_int(const Parametrisation::key_type& key) const {
    return __get<std::vector<unsigned int>>(*this, key);
}


std::vector<long> Parametrisation::get_vector_long(const Parametrisation::key_type& key) const {
    return __get<std::vector<long>>(*this, key);
}


std::vector<unsigned long> Parametrisation::get_vector_unsigned_long(const Parametrisation::key_type& key) const {
    return __get<std::vector<unsigned long>>(*this, key);
}


std::vector<float> Parametrisation::get_vector_float(const Parametrisation::key_type& key) const {
    return __get<std::vector<float>>(*this, key);
}


std::vector<double> Parametrisation::get_vector_double(const Parametrisation::key_type& key) const {
    return __get<std::vector<double>>(*this, key);
}


std::vector<std::string> Parametrisation::get_vector_string(const Parametrisation::key_type& key) const {
    return __get<std::vector<std::string>>(*this, key);
}


};  // namespace eckit::geo
