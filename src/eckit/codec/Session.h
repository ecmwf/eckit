// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstdint>
#include <string>

#include "eckit/codec/Record.h"
#include "eckit/codec/Stream.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class Session {
public:

    Session();

    ~Session();

    static bool active();

    static Record record(const std::string& path, size_t offset);

    static Record record(Stream, size_t offset);

    static void store(Stream stream);
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
