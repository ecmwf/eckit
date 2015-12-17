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

#include "hermes/FilePool.h"

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/exception/Exceptions.h"

namespace hermes {

//----------------------------------------------------------------------------------------------------------------------

static FilePool& FilePool::instance()
{
    static FilePool theInstance;
    return theInstance;
}

FilePool::FilePool() :
    filePoolSize( eckit::Resource<size_t>("filePoolSize", 64) ) {
}

FilePool::~FilePool() {
}

DataHandle& FilePool::open(const PathName& path) {

    DataHandle* dh = path.fileHandle();
}



    void close(const PathName& path);

    void list(std::ostream& os);


//----------------------------------------------------------------------------------------------------------------------

}  // namespace hermes

