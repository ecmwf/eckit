// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
