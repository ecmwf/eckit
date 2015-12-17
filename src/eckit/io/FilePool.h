/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015


#ifndef hermes_FilePool_h
#define hermes_FilePool_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"
#include "eckit/container/CacheLRU.h"

namespace eckit { class DataHandle; }

namespace hermes {

//----------------------------------------------------------------------------------------------------------------------

/// Takes ownership of the data handle
/// @invariant DataHandle is assumed opened

class DH {

    DataHandle* handle_;

public:

    DH(DataHandle* handle) : handle_(handle) {}

    ~DH() {
        handle->close();
        delete handle_;
    }
};

class FilePool : private eckit::NonCopyable {

public:

    static FilePool& instance();

    DataHandle& open(const PathName& path);

    void close(const PathName& path);

    void list(std::ostream& os);

    friend std::ostream &operator<<(std::ostream &s, const FilePool& p) { p.list(s); return s; }

private:

    FilePool();

    ~FilePool();

private:

    eckit::Mutex mutex_;

    eckit::CacheLRU< PathName, DH > pool_;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace hermes

#endif

