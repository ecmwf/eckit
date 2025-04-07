/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino

#ifndef eckit_log_FileTarget_h
#define eckit_log_FileTarget_h

#include <fstream>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileTarget : public LogTarget {
public:

    explicit FileTarget(const PathName& path, size_t bufferSize = 4 * 1024);
    virtual ~FileTarget();

private:

    void write(const char* start, const char* end) override;
    void flush() override;

    void print(std::ostream& s) const override;

private:

    std::ofstream out_;
    Buffer buffer_;  //< configurable buffer for ofstream
    PathName path_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
