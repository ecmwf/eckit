/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/DataType.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

template<> const char* Data::Type<void>::name() { return "byte"; }
template<> Data::Code Data::Type<void>::code() { return Data::BYTE; }

template<> const char* Data::Type<int>::name() { return "int"; }
template<> Data::Code Data::Type<int>::code() { return Data::INT; }

template<> const char* Data::Type<long>::name() { return "long"; }
template<> Data::Code Data::Type<long>::code() { return Data::LONG; }

template<> const char* Data::Type<float>::name() { return "float"; }
template<> Data::Code Data::Type<float>::code() { return Data::FLOAT; }

template<> const char* Data::Type<double>::name() { return "double"; }
template<> Data::Code Data::Type<double>::code() { return Data::DOUBLE; }

template<> const char* Data::Type<size_t>::name() { return "size_t"; }
template<> Data::Code Data::Type<size_t>::code() { return Data::UNSIGNED_LONG; }

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
