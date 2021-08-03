/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/CommandStream.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CommandStream::CommandStream(const std::string& name, const char* mode) :
    file_(popen(name.c_str(), mode)) {
    Log::info() << "CommandStream (" << name << ")" << std::endl;
    if (file_ == nullptr)
        throw CantOpenFile(name);
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
