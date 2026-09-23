// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/FileTarget.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileTarget::FileTarget(const PathName& path, size_t bufferSize) : buffer_(bufferSize), path_(path) {
    if (bufferSize) {
        buffer_.resize(bufferSize);
        buffer_.zero();
        out_.rdbuf()->pubsetbuf(buffer_, buffer_.size());
    }

    out_.open(path_.asString().c_str(), std::ofstream::out | std::ofstream::app);
    if (!out_) {
        throw eckit::CantOpenFile(path_.asString());
    }
}

FileTarget::~FileTarget() {
    // std::cerr << "FileTarget::~FileTarget() -- " << path_ << std::endl;
    // out_.close();
    // if (out_) {
    //     throw eckit::CloseError(path_.asString(), Here());
    // }
}

void FileTarget::write(const char* start, const char* end) {
    if (start >= end)
        return;
    out_.write(start, end - start);
}

void FileTarget::flush() {
    out_.flush();
}

void FileTarget::print(std::ostream& s) const {
    s << "FileTarget(path=" << path_ << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
