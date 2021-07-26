/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include <stdlib.h>

#include "eckit/runtime/Library.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Channel.h"
#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Library::Library(int argc, char** argv, const char* homeenv) :
    Main(argc, argv, homeenv) {}

Library::~Library() {}

Channel* Library::createChannel() const {
    return new Channel(new OStreamTarget(std::cout));
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
