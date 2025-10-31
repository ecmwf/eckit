/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#pragma once

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Feb 12

#include <cstring>

#include <algorithm>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/Zero.h"
#include "eckit/deprecated.h"

namespace eckit {

//-----------------------------------------------------------------------------

// To be used as a key or value in BTree or other file-based classed

/// FixedString<SIZE> manages a SIZE-byte char array as a relatively thin utility wrapper.
///
/// - Strings <= the given SIZE may be stored
/// - Strings shorter than the given SIZE are null-character terminated
/// - All relevant overflow and termination safety is handled internally inside the class when interacting with C
///   library functions.
///
/// @note length() returns the length of the stored string, and size() returns the number of bytes taken by the array
///       such that length() <= size().

template <int SIZE>
class FixedString {
public:

    /// Constructors
    /// @note that constructing FixedStrings initialised from another FixedString of a different SIZE actually
    ///       routes through the const std::string& constructor, via the provided implicit cast to std::string below.

    FixedString() = default;

    FixedString(const FixedString&) = default;

    FixedString& operator=(const FixedString&) = default;
   
    DEPRECATED("Use FixedString<N>::from(std::string_view) instead.")
    FixedString(const std::string&);

    DEPRECATED("Use FixedString<N>::from(std::string_view) instead.")
    FixedString(const char*);

    static FixedString from(std::string_view value);

    DEPRECATED("Use obj.assignFrom(std::string_view) instead.")
    FixedString& operator=(const std::string&);

    FixedString& assignFrom(std::string_view value);

    bool operator<(const FixedString& other) const { return ::memcmp(data_, other.data_, SIZE) < 0; }

    bool operator>(const FixedString& other) const { return ::memcmp(data_, other.data_, SIZE) > 0; }

    bool operator==(const FixedString& other) const { return ::memcmp(data_, other.data_, SIZE) == 0; }

    bool operator!=(const FixedString& other) const { return ::memcmp(data_, other.data_, SIZE) != 0; }

    bool operator>=(const FixedString& other) const { return ::memcmp(data_, other.data_, SIZE) >= 0; }

    bool operator<=(const FixedString& other) const { return ::memcmp(data_, other.data_, SIZE) <= 0; }

    /// The number of characters in the stored string,
    /// excluding the termination character if the string is shorter than SIZE.
    size_t length() const;

    bool empty() const { return length() == 0; }

    DEPRECATED("Use asStringView instead.")
    std::string asString() const;

    DEPRECATED("Will be removed without replacement.")
    operator std::string() const;

    std::string_view asStringView() const;

    DEPRECATED("Will be removed without replacement.")
    char* data() { return data_; }

    const char* data() const { return data_; }

    /// The number of bytes in the managed array (always equal to SIZE).
    size_t size() const { return SIZE; }

    static size_t static_size() { return SIZE; }

private:

    char data_[SIZE] = {};

    void print(std::ostream& s) const;

    friend std::ostream& operator<<(std::ostream& s, const FixedString& p) {
        p.print(s);
        return s;
    }
};
// TODO(kkratz): Do not forget to enforce enforce trivial copy so that zeroing is guaranteed to be safe
// static_assert(std::is_trivially_copyable_v<FixedString<1>>, "FixedString must remain trivially copyable!");

//-----------------------------------------------------------------------------

template <int SIZE>
FixedString<SIZE>::FixedString(const std::string& s) {
    ASSERT(s.length() <= SIZE && sizeof(s[0]) == 1);
    zero(data_);
    std::copy(s.begin(), s.end(), data_);
}

template <int SIZE>
FixedString<SIZE>::FixedString(const char* s) {
    ASSERT(sizeof(char) == 1 && s && strlen(s) <= SIZE);
    zero(data_);
    ::memcpy(data_, s, strlen(s));
}

template<int SIZE>
FixedString<SIZE> FixedString<SIZE>::from(std::string_view value)
{
    ASSERT(value.size() <= SIZE && sizeof(value[0] == 1));
    FixedString<SIZE> val{};
    ::memcpy(val.data_, value.data(), value.size());
}

template <int SIZE>
FixedString<SIZE>& FixedString<SIZE>::operator=(const std::string& s) {
    ASSERT(s.length() <= SIZE && sizeof(s[0]) == 1);

    ::memcpy(data_, s.c_str(), s.length());
    if (s.length() < SIZE) {
        ::memset(data_ + s.length(), 0, SIZE - s.length());
    }

    return *this;
}

template <int SIZE>
FixedString<SIZE>& FixedString<SIZE>::assignFrom(std::string_view sv) {
    ASSERT(sv.length() <= SIZE && sizeof(sv[0]) == 1);

    ::memcpy(data_, sv.data(), sv.length());
    if (sv.length() < SIZE) {
        ::memset(data_ + sv.length(), 0, SIZE - sv.length());
    }

    return *this;
}

template <int SIZE>
size_t FixedString<SIZE>::length() const {
    return std::find(data_, data_ + SIZE, 0) - data_;
}

template <int SIZE>
std::string FixedString<SIZE>::asString() const {
    return std::string(data_, data_ + length());
}

template <int SIZE>
void FixedString<SIZE>::print(std::ostream& s) const {
    s.write(data_, length());
}

template <int SIZE>
FixedString<SIZE>::operator std::string() const {
    return std::string(data_, data_ + length());
}

//-----------------------------------------------------------------------------

}  // namespace eckit
