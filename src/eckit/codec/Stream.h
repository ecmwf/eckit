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

#include <cstdint>
#include <memory>

namespace eckit {
class DataHandle;
}

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

/// @class Stream
/// @brief Handle to a shared eckit::DataHandle
///
/// Note, a Stream is not intended to be opened and closed within eckit::codec context
/// The derived classes InputFileStream and OutputFileStream automatically open and close
/// on construction an destruction.
class Stream {
public:

    /// Default constructor
    /// @post Stream is not usable but can be assigned to become valid
    Stream() = default;

    /// Constructor taking ownership of datahandle
    explicit Stream(DataHandle*);

    /// Constructor to share datahandle with a shared_ptr
    explicit Stream(std::shared_ptr<DataHandle>);

    /// Constructor referencing datahandle, no ownership is taken
    /// @note The usability depends on the usable lifetime of
    ///       the referenced datahandle
    Stream(DataHandle&);

    /// Assignment/Copy constructor sharing datahandle with other Stream
    Stream(const Stream&)            = default;
    Stream(Stream&&)                 = default;
    Stream& operator=(const Stream&) = default;
    Stream& operator=(Stream&&)      = default;

    /// Access internal DataHandle
    DataHandle& datahandle();

    /// Move position to given offset
    std::uint64_t seek(std::uint64_t offset);

    /// Return offset of current position
    std::uint64_t position();

    /// Write data of given length (bytes)
    /// @return number of bytes written
    /// @post The position is increased with number of bytes written
    std::uint64_t write(const void* data, size_t length);

    /// Read data of given length (bytes)
    /// @return number of bytes read
    /// @post The position is increased with number of bytes read
    std::uint64_t read(void* data, size_t length);

    /// Return true if pointer is valid;
    explicit operator bool() const { return ptr_ != nullptr; }

private:

    std::shared_ptr<DataHandle> shared_;
    DataHandle* ptr_{nullptr};
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
