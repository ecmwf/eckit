// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "eckit/blockfile/Format.h"
#include "eckit/filesystem/PathName.h"

namespace eckit::blockfile {

//----------------------------------------------------------------------------------------------------------------------

/// Reads newline-separated records one at a time from a block file, a zstd-compressed
/// text file or a plain text file (detected from the first bytes). Empty lines and a
/// trailing `\r` are dropped.
class RecordReader {
public:

    enum class Format {
        BlockFile,
        Zstd,
        Plain,
    };

    explicit RecordReader(const PathName& path);
    ~RecordReader();

    RecordReader(const RecordReader&)            = delete;
    RecordReader& operator=(const RecordReader&) = delete;

    /// Store the next record in `record`; false at the end of the file.
    bool next(std::string& record);

    Format format() const;

    /// Label keys of a block file (empty for other formats).
    const std::vector<LabelKey>& keys() const;

    /// Labels of the block holding the last record returned (block files only, else empty).
    const Labels& labels() const;

private:

    class Impl;
    std::unique_ptr<Impl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
