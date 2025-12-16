/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdio>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/StdPipe.h"

namespace eckit {

StdPipe::StdPipe(const std::string& name, const std::string& mode) : file_{nullptr} {
    file_ = ::popen(name.c_str(), mode.c_str());

    if (file_ == nullptr) {
        throw CantOpenFile(name);
    }
}

StdPipe::~StdPipe() {
    ASSERT_MSG(!isOpen(), "StdPipe hasn't been closed before destruction");
}

void StdPipe::close() noexcept(false) {
    if (isOpen()) {
        if (::pclose(file_) == -1) {
            throw FailedSystemCall("pclose");
        }
    }
    file_ = nullptr;
}

}  // namespace eckit
