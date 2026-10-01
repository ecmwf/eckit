// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/io/CommandStream.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CommandStream::CommandStream(const std::string& name, const char* mode) : file_(popen(name.c_str(), mode)) {
    Log::info() << "CommandStream (" << name << ")" << std::endl;
    if (file_ == nullptr) {
        throw CantOpenFile(name);
    }
}

CommandStream::~CommandStream() {
    // Somethings wrong here, throw in a dtor ??
    //	if(pclose(file_))
    //		throw WriteError("CommandStream::~CommandStream()");
}

long CommandStream::read(void* buf, long length) {
    return fread(buf, 1, length, file_);
}

long CommandStream::write(const void* buf, long length) {
    return fwrite(buf, 1, length, file_);
}

std::string CommandStream::name() const {
    return "CommandStream";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
