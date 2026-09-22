// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class Base64 {
public:

    static std::string encode(const void* data, size_t len);
    static std::string decode(const void* data, size_t len);

    template <typename T>
    static std::string encode(const T& value) {
        return encode(&value, sizeof(value));
    }

    template <typename T>
    static T decode(const std::string& in) {
        std::string decoded = decode(in.data(), in.size());
        return *reinterpret_cast<const T*>(decoded.data());
    }
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
