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


#include "eckit/codec/detail/Encoder.h"

#include "eckit/codec/Exceptions.h"

namespace eckit::codec {

size_t encode_metadata(const Encoder& encoder, Metadata& metadata) {
    ASSERT(encoder);
    return encoder.self_->encode_metadata_(metadata);
}

void encode_data(const Encoder& encoder, Data& out) {
    ASSERT(encoder);
    encoder.self_->encode_data_(out);
}

}  // namespace eckit::codec
