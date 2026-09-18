// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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
