// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/search/LoadedFile.h"


namespace eckit::geo::search {


/// k-d tree read from its cache file (built as required) into memory private to the process
class FileIO final : public LoadedFile {
public:

    // -- Constructors

    FileIO(const std::string& uid, size_t size);

    // -- Overridden methods

    MemoryUsage footprint() const override;

    // -- Class methods

    static MemoryUsage estimate(size_t size) { return {storage_size(size), 0}; }

private:

    // -- Members

    std::unique_ptr<char[]> buffer_;
    size_t buffer_size_ = 0;

    // -- Overridden methods

    char* load() override;
    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
