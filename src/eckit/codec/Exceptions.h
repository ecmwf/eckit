// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <type_traits>
#include <typeinfo>

#include "eckit/codec/detail/demangle.h"
#include "eckit/exception/Exceptions.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class Exception : public eckit::Exception {
public:

    using eckit::Exception::Exception;
    ~Exception() override;
};

//---------------------------------------------------------------------------------------------------------------------

class NotEncodable : Exception {
public:

    NotEncodable(const std::string& type_name);

    template <typename T>
    NotEncodable(const T&) : NotEncodable{demangle<typename std::decay<T>::type>()} {}

    ~NotEncodable() override;
};

//---------------------------------------------------------------------------------------------------------------------

class NotDecodable : public Exception {
public:

    NotDecodable(const std::string& type_name);

    template <typename T>
    NotDecodable(const T&) : NotDecodable{demangle<typename std::decay<T>::type>()} {}

    ~NotDecodable() override;
};

//---------------------------------------------------------------------------------------------------------------------

class InvalidRecord : public Exception {
public:

    InvalidRecord(const std::string& message) : Exception("eckit::codec::InvalidRecord: " + message) {}

    ~InvalidRecord() override;
};

//---------------------------------------------------------------------------------------------------------------------

class DataCorruption : public Exception {
public:

    DataCorruption(const std::string& message) : Exception("eckit::codec::DataCorruption: " + message) {}

    ~DataCorruption() override;
};

//---------------------------------------------------------------------------------------------------------------------

class WriteError : public Exception {
public:

    WriteError(const std::string& message) : Exception("eckit::codec::WriteError: " + message) {}

    ~WriteError() override;
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
