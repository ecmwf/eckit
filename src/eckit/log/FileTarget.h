// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
