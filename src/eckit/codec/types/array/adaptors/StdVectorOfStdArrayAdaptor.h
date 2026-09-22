// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <vector>

#include "eckit/codec/Data.h"
#include "eckit/codec/Exceptions.h"
#include "eckit/codec/Metadata.h"
#include "eckit/codec/detail/demangle.h"
#include "eckit/codec/types/array/ArrayMetadata.h"
#include "eckit/codec/types/array/ArrayReference.h"

namespace std {

//---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t N>
void interprete(const std::vector<std::array<T, N>>& vector_of_array, eckit::codec::ArrayReference& out) {
    using eckit::codec::ArrayReference;
    out = ArrayReference{vector_of_array.front().data(), {vector_of_array.size(), N}};
}

//---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t N>
void decode(const eckit::codec::Metadata& m, const eckit::codec::Data& encoded, std::vector<std::array<T, N>>& out) {
    eckit::codec::ArrayMetadata array(m);

    if (array.datatype().kind() != eckit::codec::ArrayMetadata::DataType::kind<T>()) {
        std::stringstream err;
        err << "Could not decode " << m.json() << " into std::vector<" << eckit::codec::demangle<T>() << ">. "
            << "Incompatible datatype!";
        throw eckit::codec::Exception(err.str(), Here());
    }

    if (array.rank() != 2) {
        std::stringstream err;
        err << "Could not decode " << m.json() << " into std::vector<std::array<" << eckit::codec::demangle<T>() << ","
            << N << ">>. " << "Incompatible rank!";
        throw eckit::codec::Exception(err.str(), Here());
    }

    if (array.shape(1) != N) {
        std::stringstream err;
        err << "Could not decode " << m.json() << " into std::vector<std::array<" << eckit::codec::demangle<T>() << ","
            << N << ">>. " << "Incompatible size!";
        throw eckit::codec::Exception(err.str(), Here());
    }

    const auto* data = static_cast<const std::array<T, N>*>(encoded.data());
    // std::copy(data, data + array.shape(0), out.begin());
    out.assign(data, data + array.shape(0));
}

//---------------------------------------------------------------------------------------------------------------------


}  // end namespace std
