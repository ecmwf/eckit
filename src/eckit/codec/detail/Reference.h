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
