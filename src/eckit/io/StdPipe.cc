// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/StdPipe.h"

#include <cstdio>
#include <string>

#include "eckit/exception/Exceptions.h"

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
