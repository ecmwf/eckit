// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/codec/Data.h"
#include "eckit/codec/Exceptions.h"
#include "eckit/codec/Metadata.h"
#include "eckit/codec/detail/sfinae.h"

namespace eckit::codec {


template <typename T>
struct Reference {
    const T* ref;
    explicit Reference(const T& r) : ref(&r) {}

    friend size_t encode_metadata(const Reference<T>& in, Metadata& metadata) {
        size_t size{0};
        if (not sfinae::encode_metadata(*in.ref, metadata, size)) {
            throw NotEncodable(*in.ref);
        }
        return size;
    }

    friend void encode_data(const Reference<T>& in, Data& out) {
        if (not sfinae::encode_data(*in.ref, out)) {
            throw NotEncodable(*in.ref);
        }
    }
};


}  // namespace eckit::codec
