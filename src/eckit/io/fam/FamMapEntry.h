/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamMapEntry.h
/// @author Metin Cakircali
/// @date   Mar 2026

#pragma once

#include <cstring>
#include <type_traits>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/types/FixedString.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Type returned by FamMap iterator dereference.
///
/// Encapsulates the key and value of a map entry.
/// The buffer layout is: [key (KeySize bytes)] [value data (variable length)].
/// This design allows the FamMap to store entries in FamList nodes without needing a separate structure in FAM for the
/// entry, and keeps all data in FAM for efficient access by multiple processes/threads.
template <int KeySize>
struct FamMapEntry {

    using key_type   = FixedString<KeySize>;
    using value_type = Buffer;
    using size_type  = std::size_t;

    static constexpr auto key_size = static_cast<size_type>(KeySize);

    /// Encode a key-value pair into a flat buffer for storage in FamList nodes.
    /// Layout: [key (KeySize bytes)] [value data (length bytes)]
    static Buffer encode(const key_type& key, const void* data, size_type length) {
        Buffer payload(key_size + length);
        std::memcpy(payload.data(), key.data(), key_size);
        if (length > 0) {
            void* value_ptr = static_cast<char*>(payload.data()) + key_size;
            if (data) {
                std::memcpy(value_ptr, data, length);
            }
            else {
                std::memset(value_ptr, 0, length);
            }
        }
        return payload;
    }

    static key_type decodeKey(const Buffer& buffer) {
        if (buffer.size() < key_size) {
            throw eckit::SeriousBug("FamMapEntry: buffer too small to decode key!", Here());
        }
        key_type key;
        std::memcpy(key.data(), buffer.data(), key_size);
        return key;
    }

    static value_type decodeValue(const Buffer& buffer) {
        if (buffer.size() < key_size) {
            throw eckit::SeriousBug("FamMapEntry: buffer too small to decode value!", Here());
        }
        const auto value_size = buffer.size() - key_size;
        value_type value(value_size);
        if (value_size > 0) {
            std::memcpy(value.data(), static_cast<const char*>(buffer.data()) + key_size, value_size);
        }
        return value;
    }

    /// Decode a buffer into key + value
    explicit FamMapEntry(const Buffer& buffer) : key{decodeKey(buffer)}, value{decodeValue(buffer)} {}

    key_type key;
    value_type value;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
struct IsFamMapEntry : std::false_type {};

template <int KeySize>
struct IsFamMapEntry<FamMapEntry<KeySize>> : std::true_type {};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
