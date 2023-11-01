/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <string>

#include "eckit/codec/Stream.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {
class DataHandle;
}

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

enum class Mode
{
    read,
    append,
    write,
};

//---------------------------------------------------------------------------------------------------------------------

class FileStream : public Stream {
public:
    FileStream(const PathName& path, Mode openmode);
    FileStream(const PathName& path, char openmode);
    FileStream(const PathName& path, const std::string& openmode);
};

//---------------------------------------------------------------------------------------------------------------------

class InputFileStream : public FileStream {
public:
    InputFileStream(const PathName& path);
};

//---------------------------------------------------------------------------------------------------------------------

class OutputFileStream : public FileStream {
public:
    OutputFileStream(const PathName& path, Mode openmode = Mode::write);
    OutputFileStream(const PathName& path, const std::string& openmode);
    OutputFileStream(const PathName& path, char openmode);
    void close();
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
