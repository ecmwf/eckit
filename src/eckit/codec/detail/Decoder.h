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

#include <memory>

#include "eckit/codec/Data.h"
#include "eckit/codec/Metadata.h"
#include "eckit/codec/detail/TypeTraits.h"

namespace eckit::codec {

class Decoder {
public:

    template <typename T, enable_if_decodable_t<T> = 0>
    explicit Decoder(T& value) : self_(new DecodableItem<T>(value)) {}

    friend void decode(const Metadata& metadata, const Data& data, Decoder&);
    friend void decode(const Metadata& metadata, const Data& data, Decoder&&);

private:

    struct Decodable {
        virtual ~Decodable()                               = default;
        virtual void decode_(const Metadata&, const Data&) = 0;
    };

    template <typename T>
    struct DecodableItem : Decodable {
        explicit DecodableItem(T& value) : data_(value) {}

        void decode_(const Metadata& metadata, const Data& encoded) override { decode(metadata, encoded, data_); }

        T& data_;
    };

    std::shared_ptr<Decodable> self_;
};

void decode(const Metadata&, const Data&, Decoder&);
void decode(const Metadata&, const Data&, Decoder&&);

}  // namespace eckit::codec
