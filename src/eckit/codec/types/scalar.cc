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


// Cray C++ compiler should not try to optimize this code
#ifdef _CRAYC
#pragma _CRI noopt
#endif

// GNU C++ compiler (version 11) should not try to optimize this code
#if defined(__GNUC__) && !defined(__NVCOMPILER) && !defined(__clang__)
#pragma GCC optimize("O0")
#endif

#include "eckit/codec/types/scalar.h"

#include <cstdint>
#include <string>

#include "eckit/codec/Data.h"
#include "eckit/codec/Metadata.h"
#include "eckit/codec/detail/Base64.h"
#include "eckit/codec/detail/DataType.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/ByteSwap.h"

namespace eckit::codec {


//---------------------------------------------------------------------------------------------------------------------

template <typename T>
void decode_scalar(const Metadata& metadata, T& value) {
    ASSERT(metadata.getString("type") == "scalar");
    ASSERT(metadata.getString("datatype") == DataType::str<T>());
    metadata.get("value", value);
}

template <typename T>
void decode_scalar_b64(const Metadata& metadata, T& value) {
    ASSERT(metadata.getString("type") == "scalar");
    ASSERT(metadata.getString("datatype") == DataType::str<T>());
    std::string base64 = metadata.getString("base64");
    T value_ns         = Base64::decode<T>(base64);
    if (Endian::native == Endian::little) {
        T tmp = value_ns;
        eckit::byteswap(tmp);
        value_ns = tmp;
    }
    value = value_ns;
}

//---------------------------------------------------------------------------------------------------------------------

template <typename T>
void encode_scalar_metadata(const T& value, Metadata& out) {
    out.set("type", "scalar");
    out.set("datatype", DataType::str<T>());
    out.set("value", value);
}

inline void encode_scalar_metadata(const unsigned long& value, Metadata& out) {
    out.set("type", "scalar");
    out.set("datatype", DataType::str<size_t>());
    out.set("value", static_cast<size_t>(value));
}

inline void encode_scalar_metadata(const unsigned long long& value, Metadata& out) {
    out.set("type", "scalar");
    out.set("datatype", DataType::str<size_t>());
    out.set("value", static_cast<size_t>(value));
}

template <typename T>
size_t encode_scalar_metadata_b64(const T& value, Metadata& out) {
    encode_scalar_metadata(value, out);
    T value_ns = value;
    if (Endian::native == Endian::little) {
        eckit::byteswap(value_ns);
    }
    out.set("base64", Base64::encode(value_ns));
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------

size_t encode_metadata(const int& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

size_t encode_metadata(const long& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

size_t encode_metadata(const long long& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

size_t encode_metadata(const unsigned long& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

size_t encode_metadata(const unsigned long long& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

size_t encode_metadata(const float& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

size_t encode_metadata(const double& value, Metadata& out) {
    return encode_scalar_metadata_b64(value, out);
}

//---------------------------------------------------------------------------------------------------------------------

void encode_data(const int&, Data&) {}
void encode_data(const long&, Data&) {}
void encode_data(const long long&, Data&) {}
void encode_data(const unsigned long&, Data&) {}
void encode_data(const unsigned long long&, Data&) {}
void encode_data(const float&, Data&) {}
void encode_data(const double&, Data&) {}

//---------------------------------------------------------------------------------------------------------------------

void decode(const Metadata& metadata, const Data&, int& value) {
    decode_scalar(metadata, value);
}
void decode(const Metadata& metadata, const Data&, long& value) {
    decode_scalar(metadata, value);
}
void decode(const Metadata& metadata, const Data&, long long& value) {
    decode_scalar(metadata, value);
}
void decode(const Metadata& metadata, const Data&, unsigned long& value) {
    decode_scalar(metadata, value);
}
void decode(const Metadata& metadata, const Data&, unsigned long long& value) {
    decode_scalar_b64(metadata, value);
}
void decode(const Metadata& metadata, const Data&, float& value) {
    decode_scalar_b64(metadata, value);
}
void decode(const Metadata& metadata, const Data&, double& value) {
    decode_scalar_b64(metadata, value);
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
