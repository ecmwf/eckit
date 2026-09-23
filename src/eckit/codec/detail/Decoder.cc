// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/codec/detail/Decoder.h"

namespace eckit::codec {

void decode(const Metadata& metadata, const Data& data, Decoder& decoder) {
    decoder.self_->decode_(metadata, data);
}

void decode(const Metadata& metadata, const Data& data, Decoder&& decoder) {
    decoder.self_->decode_(metadata, data);
}


}  // namespace eckit::codec
