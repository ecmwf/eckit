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


#include "eckit/codec/detail/Decoder.h"

namespace eckit::codec {

void decode(const Metadata& metadata, const Data& data, Decoder& decoder) {
    decoder.self_->decode_(metadata, data);
}

void decode(const Metadata& metadata, const Data& data, Decoder&& decoder) {
    decoder.self_->decode_(metadata, data);
}


}  // namespace eckit::codec
