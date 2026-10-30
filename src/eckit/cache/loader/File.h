// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/cache/Loader.h"


namespace eckit::cache::loader {


/// Content read into private memory: always available, at the cost of a copy per process
class File final : public Loader {
public:

    // -- Constructors

    File(const PathName&, const Options&);

    // -- Class methods

    static MemoryUsage estimate(size_t size) { return {size, 0}; }

    // -- Overridden methods

    const void* address() const override { return buffer_.get(); }
    size_t size() const override { return size_; }
    MemoryUsage footprint() const override { return estimate(size_); }

private:

    // -- Members

    std::unique_ptr<char[]> buffer_;
    size_t size_ = 0;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::cache::loader
