/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <complex>
#include <utility>

#include "eckit/mpi/DataType.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

template <>
const char* Data::Type<void>::name() {
    return "byte";
}
template <>
Data::Code Data::Type<void>::code() {
    return Data::BYTE;
}
template <>
size_t Data::Type<void>::size() {
    return 1;
}

template <>
const char* Data::Type<char>::name() {
    return "char";
}
template <>
Data::Code Data::Type<char>::code() {
    return Data::CHAR;
}
template <>
size_t Data::Type<char>::size() {
    return sizeof(char);
}

template <>
const char* Data::Type<int>::name() {
    return "int";
}
template <>
Data::Code Data::Type<int>::code() {
    return Data::INT;
}
template <>
size_t Data::Type<int>::size() {
    return sizeof(int);
}

template <>
const char* Data::Type<long>::name() {
    return "long";
}
template <>
Data::Code Data::Type<long>::code() {
    return Data::LONG;
}
template <>
size_t Data::Type<long>::size() {
    return sizeof(long);
}

template <>
const char* Data::Type<long long>::name() {
    return "long long";
}
template <>
Data::Code Data::Type<long long>::code() {
    return Data::LONG_LONG;
}
template <>
size_t Data::Type<long long>::size() {
    return sizeof(long long);
}

template <>
const char* Data::Type<float>::name() {
    return "float";
}
template <>
Data::Code Data::Type<float>::code() {
    return Data::FLOAT;
}
template <>
size_t Data::Type<float>::size() {
    return sizeof(float);
}

template <>
const char* Data::Type<double>::name() {
    return "double";
}
template <>
Data::Code Data::Type<double>::code() {
    return Data::DOUBLE;
}
template <>
size_t Data::Type<double>::size() {
    return sizeof(double);
}

template <>
const char* Data::Type<size_t>::name() {
    return "size_t";
}
template <>
Data::Code Data::Type<size_t>::code() {
    return Data::UNSIGNED_LONG;
}
template <>
size_t Data::Type<size_t>::size() {
    return sizeof(size_t);
}

//----------------------------------------------------------------------------------------------------------------------

template <>
const char* Data::Type<std::pair<int, int> >::name() {
    return "<int,int>";
}
template <>
Data::Code Data::Type<std::pair<int, int> >::code() {
    return Data::INT_INT;
}
template <>
size_t Data::Type<std::pair<int, int> >::size() {
    return sizeof(std::pair<int, int>);
}

template <>
const char* Data::Type<std::pair<long, long> >::name() {
    return "<long,long>";
}
template <>
Data::Code Data::Type<std::pair<long, long> >::code() {
    return Data::TWO_LONG;
}
template <>
size_t Data::Type<std::pair<long, long> >::size() {
    return sizeof(std::pair<long, long>);
}

template <>
const char* Data::Type<std::pair<long long, long long> >::name() {
    return "<long,long>";
}
template <>
Data::Code Data::Type<std::pair<long long, long long> >::code() {
    return Data::TWO_LONG_LONG;
}
template <>
size_t Data::Type<std::pair<long long, long long> >::size() {
    return sizeof(std::pair<long long, long long>);
}

template <>
const char* Data::Type<std::pair<long, int> >::name() {
    return "<long,int>";
}
template <>
Data::Code Data::Type<std::pair<long, int> >::code() {
    return Data::LONG_INT;
}
template <>
size_t Data::Type<std::pair<long, int> >::size() {
    return sizeof(std::pair<long, int>);
}

template <>
const char* Data::Type<std::pair<float, int> >::name() {
    return "<float,int>";
}
template <>
Data::Code Data::Type<std::pair<float, int> >::code() {
    return Data::FLOAT_INT;
}
template <>
size_t Data::Type<std::pair<float, int> >::size() {
    return sizeof(std::pair<float, int>);
}

template <>
const char* Data::Type<std::pair<double, int> >::name() {
    return "<double,int>";
}
template <>
Data::Code Data::Type<std::pair<double, int> >::code() {
    return Data::DOUBLE_INT;
}
template <>
size_t Data::Type<std::pair<double, int> >::size() {
    return sizeof(std::pair<double, int>);
}

template <>
const char* Data::Type<std::complex<double> >::name() {
    return "complex<double>";
}
template <>
Data::Code Data::Type<std::complex<double> >::code() {
    return Data::DOUBLE_COMPLEX;
}
template <>
size_t Data::Type<std::complex<double> >::size() {
    return sizeof(std::complex<double>);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
