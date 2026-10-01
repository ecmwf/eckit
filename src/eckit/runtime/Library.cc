// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// #include <cstdlib>

#include "eckit/runtime/Library.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Channel.h"
#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Library::Library(int argc, char** argv, const char* homeenv) : Main(argc, argv, homeenv) {}

Library::~Library() {}

Channel* Library::createChannel() const {
    return new Channel(new OStreamTarget(std::cout));
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
