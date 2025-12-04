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

// File Padded.h
// Baudouin Raoult - ECMWF Jan 97

#include <cstddef>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// Allows to pad T to the next smallest multiple of size larger or equal than sizeof(T).
/// @warning This class is often written to disk! Any change must ensure binary compatibility.
template <class T, int size>
class Padded : public T {
private:

    static constexpr auto align_ = size;
    static constexpr auto osize_ = sizeof(T);

    // Add the padding
    char padding_[((size_t(osize_) + size_t(align_) - 1) / align_) * align_ - osize_]{};
};


//-----------------------------------------------------------------------------

}  // namespace eckit
