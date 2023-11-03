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

#include <iomanip>
#include <sstream>
#include <string>

#include "eckit/codec/Exceptions.h"
#include "eckit/codec/FileStream.h"
#include "eckit/codec/Record.h"
#include "eckit/codec/RecordItemReader.h"
#include "eckit/codec/RecordPrinter.h"
#include "eckit/codec/RecordReader.h"
#include "eckit/codec/RecordWriter.h"
#include "eckit/codec/Session.h"
#include "eckit/codec/Stream.h"
#include "eckit/codec/detail/Link.h"
#include "eckit/codec/detail/Reference.h"
#include "eckit/codec/detail/StaticAssert.h"
#include "eckit/codec/detail/sfinae.h"
#include "eckit/codec/types/array.h"
#include "eckit/codec/types/scalar.h"
#include "eckit/codec/types/string.h"


namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

inline Link link(const std::string& uri) {
    return Link{uri};
}

//---------------------------------------------------------------------------------------------------------------------

template <typename T, disable_if_interpretable_t<T, ArrayReference> = 0>
Reference<T> ref(const T& x, tag::enable_static_assert = tag::enable_static_assert()) {
    static_assert(is_encodable<T>(),
                  "in ref(const Value&)"
                  "\n"
                  "\n     Static assertion failed"
                  "\n     -----------------------"
                  "\n"
                  "\n     Cannot encode values of referenced type."
                  "\n"
                  "\n     Implement the functions"
                  "\n"
                  "\n         void encode_data(const Value& in, Data& out);"
                  "\n         size_t encode_metadata(const Value& value, Metadata& metadata);"
                  "\n"
                  "\n     or alternatively a conversion function to types::ArrayView"
                  "\n"
                  "\n         void interprete(const Value& in, types::ArrayView& out)"
                  "\n"
                  "\n     Rules of argument-dependent-lookup apply."
                  "\n     --> Functions need to be declared in namespace of any of the arguments."
                  "\n"
                  "\n     Note, turn this into a runtime exception by calling this function instead:"
                  "\n"
                  "\n        ref(const T&, no_static_assert() )"
                  "\n");
    return Reference<T>(x);
}


template <typename T, disable_if_interpretable_t<T, ArrayReference> = 0>
Reference<T> ref(const T& x, tag::disable_static_assert) {
    if (not is_encodable<T>()) {
        throw NotEncodable(x);
    }
    return Reference<T>(x);
}


template <typename T, enable_if_interpretable_t<T, ArrayReference> = 0>
ArrayReference ref(const T& x, tag::enable_static_assert = tag::enable_static_assert()) {
    ArrayReference w;
    interprete(x, w);
    return w;
}

//---------------------------------------------------------------------------------------------------------------------

template <typename T>
RecordItem copy(T&& value, tag::disable_static_assert) {
    return RecordItem(std::forward<T>(value), tag::disable_static_assert());
}

template <typename T>
RecordItem copy(T&& value) {
    return RecordItem(std::forward<T>(value));
}

//---------------------------------------------------------------------------------------------------------------------

template <typename T>
void encode(const T& in, Metadata& metadata, Data& data, tag::enable_static_assert = tag::enable_static_assert()) {
    auto referenced = ref(in, tag::enable_static_assert());
    sfinae::encode_metadata(referenced, metadata);
    sfinae::encode_data(referenced, data);
}

template <typename T>
void encode(const T& in, Metadata& metadata, Data& data, tag::disable_static_assert) {
    auto referenced = ref(in, tag::disable_static_assert());
    sfinae::encode_metadata(referenced, metadata);
    sfinae::encode_data(referenced, data);
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
