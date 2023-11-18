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


#pragma once

#include <array>
#include <type_traits>

#include "eckit/codec/Data.h"
#include "eckit/codec/Exceptions.h"
#include "eckit/codec/Metadata.h"
#include "eckit/codec/types/array/ArrayMetadata.h"
#include "eckit/codec/types/array/ArrayReference.h"

namespace std {

//---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t N>
void interprete(const std::array<T, N>& vector, eckit::codec::ArrayReference& out) {
    out = eckit::codec::ArrayReference{vector.data(), {N}};
}

//---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t N>
void decode(const eckit::codec::Metadata& m, const eckit::codec::Data& encoded, std::array<T, N>& out) {
    eckit::codec::ArrayMetadata array(m);
    if (array.datatype().kind() != eckit::codec::ArrayMetadata::DataType::kind<T>()) {
        std::stringstream err;
        err << "Could not decode " << m.json() << " into std::vector<" << typeid(typename std::decay<T>::type).name()
            << ">. " << "Incompatible datatype!";

        throw eckit::codec::Exception(err.str(), Here());
    }
    if (array.size() != N) {
        std::stringstream err;
        err << "Could not decode " << m.json() << " into std::array<" << typeid(typename std::decay<T>::type).name()
            << "," << N << ">. " << "Incompatible size!";
        throw eckit::codec::Exception(err.str(), Here());
    }
    const T* data = static_cast<const T*>(encoded.data());
    std::copy(data, data + N, out.begin());
}

//---------------------------------------------------------------------------------------------------------------------

}  // end namespace std
