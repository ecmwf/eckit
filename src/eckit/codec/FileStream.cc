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


#include "eckit/codec/FileStream.h"

#include "eckit/codec/Session.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/PooledHandle.h"

namespace eckit::codec {

namespace {

//---------------------------------------------------------------------------------------------------------------------

/// DataHandle that implements file IO for reading and writing
/// Main differences with eckit::FileHandle:
///   - Automatic opening and closing of file
///   - Openmode argument:
///       * read: for reading
///       * write: for writing, will overwrite existing file
///       * append: for appending implemented via write and seek to eof.
class FileHandle : public eckit::FileHandle {
public:
    FileHandle(const PathName& path, char openmode) : eckit::FileHandle(path, openmode == 'a' /*overwrite*/) {
        if (openmode == 'r') {
            openForRead();
        }
        else if (openmode == 'w' || (openmode == 'a' && not path.exists())) {
            openForWrite(0);
        }
        else if (openmode == 'a') {
            openForWrite(path.size());
            seek(Offset(path.size()));
        }
    }

    void close() override {
        if (not closed_) {
            eckit::FileHandle::close();
            closed_ = true;
        }
    }

    FileHandle(const PathName& path, Mode openmode) :
        FileHandle(path, openmode == Mode::read    ? 'r'
                         : openmode == Mode::write ? 'w'
                                                   : 'a') {}

    FileHandle(const PathName& path, const std::string& openmode) : FileHandle(path, openmode[0]) {}

    FileHandle(const FileHandle&) = delete;
    FileHandle(FileHandle&&)      = delete;

    ~FileHandle() override { close(); }

    void operator=(const FileHandle&) = delete;
    void operator=(FileHandle&&)      = delete;

private:
    bool closed_{false};
};

//---------------------------------------------------------------------------------------------------------------------

/// DataHandle that implements file reading only.
/// Internally there is a registry of opened files which avoids
/// opening the same file multiple times.
/// Note that close() will not actually close the file when there
/// is another PooledHandle referencing the same file.
///
/// Main difference with eckit::PooledHandle
///   - Automatic opening and closing of file
class PooledHandle : public eckit::PooledHandle {
public:
    explicit PooledHandle(const PathName& path) : eckit::PooledHandle(path), path_(path) { openForRead(); }

    PooledHandle(const PooledHandle&) = delete;
    PooledHandle(PooledHandle&&)      = delete;

    ~PooledHandle() override { close(); }

    void operator=(const PooledHandle&) = delete;
    void operator=(PooledHandle&&)      = delete;

    PathName path_;
};

}  // namespace

//---------------------------------------------------------------------------------------------------------------------

FileStream::FileStream(const PathName& path, char openmode) :
    Stream(openmode == 'r' ? static_cast<DataHandle*>(new PooledHandle(path)) : new FileHandle(path, openmode)) {
    if (openmode == 'r') {
        // Keep the PooledHandle alive until the end of active session
        Session::store(*this);
    }
}

FileStream::FileStream(const PathName& path, Mode openmode) :
    FileStream(path, openmode == Mode::read    ? 'r'
                     : openmode == Mode::write ? 'w'
                                               : 'a') {}

FileStream::FileStream(const PathName& path, const std::string& openmode) : FileStream(path, openmode[0]) {}

//---------------------------------------------------------------------------------------------------------------------

InputFileStream::InputFileStream(const PathName& path) : FileStream(path, Mode::read) {}

//---------------------------------------------------------------------------------------------------------------------

OutputFileStream::OutputFileStream(const PathName& path, Mode openmode) : FileStream(path, openmode) {}

OutputFileStream::OutputFileStream(const PathName& path, const std::string& openmode) : FileStream(path, openmode) {}

OutputFileStream::OutputFileStream(const PathName& path, char openmode) : FileStream(path, openmode) {}

void OutputFileStream::close() {
    datahandle().close();
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
