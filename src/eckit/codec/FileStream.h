// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/codec/Stream.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {
class DataHandle;
}

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

enum class Mode {
    read,
    append,
    write,
};

//---------------------------------------------------------------------------------------------------------------------

class FileStream : public Stream {
public:

    FileStream(const PathName&, Mode openmode);
    FileStream(const PathName&, char openmode);
    FileStream(const PathName&, const std::string& openmode);
};

//---------------------------------------------------------------------------------------------------------------------

class InputFileStream : public FileStream {
public:

    explicit InputFileStream(const PathName&);
};

//---------------------------------------------------------------------------------------------------------------------

class OutputFileStream : public FileStream {
public:

    explicit OutputFileStream(const PathName&, Mode openmode = Mode::write);
    OutputFileStream(const PathName&, const std::string& openmode);
    OutputFileStream(const PathName&, char openmode);
    void close();
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
