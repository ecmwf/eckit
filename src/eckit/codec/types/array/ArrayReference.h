// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/codec/Data.h"
#include "eckit/codec/Metadata.h"
#include "eckit/codec/types/array/ArrayMetadata.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class ArrayReference : public ArrayMetadata {
public:

    ArrayReference() = default;

    ArrayReference(const void* data, DataType, const ArrayShape&);

    template <typename T>
    ArrayReference(const T* data, const ArrayShape& shape) :
        ArrayMetadata(DataType::create<T>(), shape), data_(const_cast<T*>(data)) {}

    ArrayReference(ArrayReference&&);

    ArrayReference& operator=(ArrayReference&&);

    void* data() const { return data_; }

    friend void decode(const Metadata&, const Data&, ArrayReference&);

private:

    void* data_{nullptr};
};

//---------------------------------------------------------------------------------------------------------------------

size_t encode_metadata(const ArrayReference& value, Metadata& out);

void encode_data(const ArrayReference& value, Data& out);

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
